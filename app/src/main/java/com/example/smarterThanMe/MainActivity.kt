package com.example.smarterThanMe

import android.Manifest
import android.annotation.SuppressLint
import android.content.Context
import android.content.Intent
import android.content.pm.PackageManager
import android.net.Uri
import android.os.Bundle
import android.os.Environment
import android.provider.MediaStore
import android.widget.Toast
import androidx.appcompat.app.AppCompatActivity
import androidx.core.app.ActivityCompat
import androidx.core.content.ContextCompat
import androidx.core.content.FileProvider
import com.example.smarterThanMe.databinding.ActivityMainBinding
import com.theartofdev.edmodo.cropper.CropImage
import java.io.File
import java.text.SimpleDateFormat
import java.util.*

const val RESULT_EXCEPTION = 42
const val RESULT_FILE_EXCEPTION = 60

class MainActivity : AppCompatActivity() {
    private val callCamera = 1
    private val reqPerm = 2
    private val callStorage = 3
    private val callSetter = 4

    private lateinit var lastCameraUri: Uri
    private lateinit var binding: ActivityMainBinding

    @SuppressLint("QueryPermissionsNeeded")
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        binding = ActivityMainBinding.inflate(layoutInflater)
        val view = binding.root
        setContentView(view)

        val storageDir = getExternalFilesDir(Environment.DIRECTORY_PICTURES)
        val lastResult = File(storageDir, "lastResult")
        if (storageDir != null && lastResult.exists() && lastResult.length() > 0) {
            binding.greeting.text = ""
            binding.savedImageView.setImageURI(Uri.fromFile(lastResult))
        }

        binding.cameraButton.setOnClickListener {
            val photoFile = createImageFile(this) ?: return@setOnClickListener
            lastCameraUri = FileProvider.getUriForFile(
                    this, "com.example.smarterThanMe.fileprovider", photoFile)
            try {
                val cameraIntent = Intent(MediaStore.ACTION_IMAGE_CAPTURE)
                if (cameraIntent.resolveActivity(packageManager) == null) {
                    onActivityResult(callCamera, RESULT_EXCEPTION, null)
                }
                cameraIntent.putExtra(MediaStore.EXTRA_OUTPUT, lastCameraUri)
                startActivityForResult(cameraIntent, callCamera)
            } catch (e: Exception) {
                onActivityResult(callCamera, RESULT_EXCEPTION, null)
            }
        }

        binding.galleryButton.setOnClickListener {
            val status = ContextCompat.checkSelfPermission(
                    this, Manifest.permission.READ_EXTERNAL_STORAGE)
            if (status == PackageManager.PERMISSION_DENIED) {
                ActivityCompat.requestPermissions(
                        this,
                        arrayOf(Manifest.permission.READ_EXTERNAL_STORAGE),
                        reqPerm)
            } else onRequestPermissionsResult(
                    reqPerm,
                    arrayOf(Manifest.permission.READ_EXTERNAL_STORAGE),
                    intArrayOf(PackageManager.PERMISSION_GRANTED))
        }
    }

    override fun onRequestPermissionsResult(
        requestCode: Int,
        permissions: Array<out String>,
        grantResults: IntArray
    ) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults)
        if (grantResults[0] == PackageManager.PERMISSION_DENIED) return
        if (requestCode == reqPerm) {
            try {
                val galleryIntent = Intent(Intent.ACTION_PICK)
                galleryIntent.setType("image/*")
                startActivityForResult(galleryIntent, callStorage)
            } catch (e: Exception) {
                onActivityResult(callStorage, RESULT_EXCEPTION, null)
            }
        }
    }

    override fun onActivityResult(requestCode: Int, resultCode: Int, data: Intent?) {
        super.onActivityResult(requestCode, resultCode, data)
        if (resultCode != RESULT_OK) {
            val errorMsg = when (resultCode) {
                RESULT_CANCELED -> ""
                RESULT_EXCEPTION -> when (requestCode) {
                    callSetter -> "Не удалось распознать лабиринт"
                    callCamera -> "Ошибка камеры"
                    callStorage -> "Ошибка галереи"
                    else -> "Упс, что-то сломалось..."
                }
                RESULT_FILE_EXCEPTION -> "Ошибка создания файла"
                else -> "Упс, что-то сломалось..."
            }
            if (errorMsg.isNotEmpty())
                Toast.makeText(this, errorMsg, Toast.LENGTH_LONG).show()
            return
        }
        when (requestCode) {
            callCamera -> CropImage.activity(lastCameraUri).start(this)
            callStorage -> if (data != null) CropImage.activity(data.data).start(this)
            CropImage.CROP_IMAGE_ACTIVITY_REQUEST_CODE -> {
                val result = CropImage.getActivityResult(data)
                val pointIntent = Intent(this, PointSetter::class.java)
                pointIntent.putExtra("uri", result.uri)
                startActivityForResult(pointIntent, callSetter)
            }
            callSetter -> if (data != null) {
                binding.greeting.text = ""
                binding.savedImageView.setImageURI(data.getParcelableExtra("uri"))
            }
        }
    }
}

@SuppressLint("SimpleDateFormat")
fun createImageFile(context: Context): File? = try {
    val timeStamp: String = SimpleDateFormat("yyyy-MM-dd_HH:mm:ss").format(Date())
    val storageDir: File? = context.getExternalFilesDir(Environment.DIRECTORY_PICTURES)
    val file = File(storageDir, "maze_${timeStamp}.jpg")
    file.createNewFile()
    file
} catch (e: Exception) {
    null
}