package com.example.smarterThanMe

import android.annotation.SuppressLint
import android.content.Intent
import android.graphics.Bitmap
import android.graphics.Canvas
import android.graphics.Paint
import android.graphics.drawable.BitmapDrawable
import android.net.Uri
import android.os.Bundle
import android.util.Log
import android.view.View
import androidx.appcompat.app.AppCompatActivity
import androidx.core.content.res.ResourcesCompat
import androidx.core.graphics.createBitmap
import com.example.smarterThanMe.databinding.ActivityPointSetterBinding
import java.io.File
import java.io.FileOutputStream

enum class State {
    PICKING_START, PICKED_START, PICKING_FINISH, PICKED_FINISH
}

external fun solve(bitmapIn: Bitmap, bitmapOut: Bitmap,
                   x1: Float, y1: Float, x2: Float, y2: Float)

class PointSetter : AppCompatActivity() {
    private lateinit var binding: ActivityPointSetterBinding

    @SuppressLint("ClickableViewAccessibility")
    override fun onCreate(savedInstanceState: Bundle?) {
        System.loadLibrary("native-lib")
        super.onCreate(savedInstanceState)
        binding = ActivityPointSetterBinding.inflate(layoutInflater)
        val view = binding.root
        setContentView(view)

        binding.imageView.setImageURI(intent.getParcelableExtra("uri"))

        var state: State = State.PICKING_START
        var pickedStartX = 0F
        var pickedStartY = 0F
        var pickedFinishX = 0F
        var pickedFinishY = 0F

        val myBitmap = (binding.imageView.drawable as BitmapDrawable).bitmap
        val img = myBitmap.copy(myBitmap.config,true);
        val tmpBitmap = Bitmap.createBitmap(
                myBitmap.width, myBitmap.height, Bitmap.Config.RGB_565)
        val drawColor = ResourcesCompat.getColor(resources, R.color.green_700, null)
        val paint = Paint(Paint.ANTI_ALIAS_FLAG).apply {
            color = drawColor
            style = Paint.Style.FILL
        }

        binding.imageView.setOnTouchListener { v, event ->
            val x = event.x * myBitmap.width / v.measuredWidth
            val y = event.y * myBitmap.height / v.measuredHeight
            Log.d("params", "$x, $y, ${v.layoutParams.width}, ${v.layoutParams.height}")
            val tmpCanvas = Canvas(tmpBitmap)
            tmpCanvas.drawBitmap(myBitmap, 0F, 0F, null)
            tmpCanvas.drawCircle(x, y, 5F * resources.displayMetrics.density, paint)
            binding.imageView.setImageBitmap(tmpBitmap)
            if (state == State.PICKING_START || state == State.PICKED_START) {
                state = State.PICKED_START
                pickedStartX = x
                pickedStartY = y
            } else {
                tmpCanvas.drawCircle(
                        pickedStartX, pickedStartY, 5F * resources.displayMetrics.density, paint)
                state = State.PICKED_FINISH
                pickedFinishX = x
                pickedFinishY = y
            }
            binding.doneButton.visibility = View.VISIBLE
            true
        }

        binding.doneButton.setOnClickListener {
            if (state == State.PICKED_START) {
                state = State.PICKING_FINISH
                binding.pointText.setText(R.string.pointText2)
                binding.doneButton.visibility = View.GONE
            }
            if (state == State.PICKED_FINISH) {
                val input = img
                val output = createBitmap(input.width, input.height, Bitmap.Config.ARGB_8888)
                val intent = Intent()
                try {
                    solve(input, output, pickedStartX, pickedStartY, pickedFinishX, pickedFinishY)
                } catch (e: Throwable) {
                    setResult(MAZE_EXCEPTION, intent)
                    finish()
                }
                val resultFile: File? = createImageFile(this)
                if (resultFile == null) {
                    setResult(FILE_EXCEPTION, intent)
                    finish()
                }
                val stream = FileOutputStream(resultFile)
                output.compress(Bitmap.CompressFormat.JPEG, 100, stream)
                intent.putExtra("uri", Uri.fromFile(resultFile))
                setResult(RESULT_OK, intent)
                finish()
            }
        }
    }
}