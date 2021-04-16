package com.example.smarterThanMe

import android.Manifest
import android.content.ActivityNotFoundException
import android.content.Intent
import android.content.pm.PackageManager
import android.graphics.Bitmap
import android.net.Uri
import android.os.Bundle
import android.os.Environment
import android.provider.MediaStore
import android.widget.Toast
import androidx.appcompat.app.AppCompatActivity
import androidx.core.app.ActivityCompat
import androidx.core.content.ContextCompat
import androidx.core.content.FileProvider
import com.theartofdev.edmodo.cropper.CropImage
import kotlinx.android.synthetic.main.activity_main.*
import java.io.File
import java.text.SimpleDateFormat
import java.util.*


class MainActivity : AppCompatActivity() {
    private val callCamera = 1
    private val reqStorPerm = 2
    private val callStorage = 3

    private lateinit var lastCameraUri: Uri

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        cameraButton.setOnClickListener {
            val photoFile = createImageFile() ?: return@setOnClickListener
            lastCameraUri = FileProvider.getUriForFile(
                    this, "com.example.smarterThanMe.fileprovider", photoFile)
            try {
                val cameraIntent = Intent(MediaStore.ACTION_IMAGE_CAPTURE)
                if (cameraIntent.resolveActivity(packageManager) == null) {
                    val errorMsg = "Ошибка камеры"
                    Toast.makeText(this, errorMsg, Toast.LENGTH_LONG).show()
                }
                cameraIntent.putExtra(MediaStore.EXTRA_OUTPUT, lastCameraUri)
                startActivityForResult(cameraIntent, callCamera)
            } catch (e: Exception) {
                val errorMsg = "Ошибка камеры"
                Toast.makeText(this, errorMsg, Toast.LENGTH_LONG).show()
            }
        }

        galleryButton.setOnClickListener {
            val status = ContextCompat.checkSelfPermission(
                    this, Manifest.permission.READ_EXTERNAL_STORAGE)
            if (status == PackageManager.PERMISSION_DENIED) {
                ActivityCompat.requestPermissions(
                        this,
                        arrayOf(Manifest.permission.READ_EXTERNAL_STORAGE),
                        reqStorPerm)
            } else onRequestPermissionsResult(
                    reqStorPerm,
                    arrayOf(Manifest.permission.READ_EXTERNAL_STORAGE),
                    intArrayOf(PackageManager.PERMISSION_GRANTED))
        }
    }

    private fun createImageFile(): File? = try {
        val timeStamp: String = SimpleDateFormat("yyyy-MM-dd_HH:mm:ss").format(Date())
        val storageDir: File? = getExternalFilesDir(Environment.DIRECTORY_PICTURES)
        val file = File(storageDir, "maze_${timeStamp}.jpg")
        file.createNewFile()
        file
    } catch (e: Exception) {
        val errorMsg = "Ошибка создания файла"
        Toast.makeText(this, errorMsg, Toast.LENGTH_LONG).show()
        null
    }

    override fun onRequestPermissionsResult(
        requestCode: Int,
        permissions: Array<out String>,
        grantResults: IntArray
    ) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults)
        if (grantResults[0] == PackageManager.PERMISSION_DENIED) return
        if (requestCode == reqStorPerm) {
            try {
                val photoFile = createImageFile() ?: return
                val photoURI: Uri = FileProvider.getUriForFile(
                        this, "com.example.smarterThanMe.fileprovider", photoFile)
                val galleryIntent = Intent(Intent.ACTION_PICK)
                galleryIntent.setType("image/*")
                startActivityForResult(galleryIntent, callStorage)
            } catch (e: Exception) {
                val errorMsg = "Ошибка галереи"
                Toast.makeText(this, errorMsg, Toast.LENGTH_LONG).show()
            }
        }
    }

    override fun onActivityResult(requestCode: Int, resultCode: Int, data: Intent?) {
        super.onActivityResult(requestCode, resultCode, data)
        if (resultCode != RESULT_OK) {
            val errorMsg = "Упс, что-то сломалось..."
            Toast.makeText(this, errorMsg, Toast.LENGTH_LONG).show()
            return
        }
        if (data == null) return
        if (requestCode == callCamera) {
            CropImage.activity(lastCameraUri).start(this)
        } else if (requestCode == callStorage) {
            CropImage.activity(data.data).start(this)
        } else if (requestCode == CropImage.CROP_IMAGE_ACTIVITY_REQUEST_CODE) {
            val result = CropImage.getActivityResult(data)
            val pointIntent = Intent(this, PointSetter::class.java)
            pointIntent.putExtra("uri", result.uri)
            startActivity(pointIntent)
        }
    }
}