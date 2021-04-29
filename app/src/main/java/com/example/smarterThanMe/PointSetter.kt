package com.example.smarterThanMe

import android.annotation.SuppressLint
import android.graphics.Bitmap
import android.graphics.Canvas
import android.graphics.Paint
import android.graphics.drawable.BitmapDrawable
import android.os.Bundle
import android.view.View
import androidx.appcompat.app.AppCompatActivity
import androidx.core.content.res.ResourcesCompat
import com.example.smarterThanMe.databinding.ActivityPointSetterBinding

enum class State {
    PICKING_START, PICKED_START, PICKING_FINISH, PICKED_FINISH
}

class PointSetter : AppCompatActivity() {
    private lateinit var binding: ActivityPointSetterBinding

    @SuppressLint("ClickableViewAccessibility")
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        binding = ActivityPointSetterBinding.inflate(layoutInflater)
        val view = binding.root
        setContentView(view)

        binding.imageView.setImageURI(intent.getParcelableExtra("uri"))

        var state: State = State.PICKING_START
        var pickedStartX: Float
        var pickedStartY: Float
        var pickedFinishX: Float
        var pickedFinishY: Float

        binding.imageView.setOnTouchListener { _, event ->
            val myBitmap = Bitmap.createBitmap(
                    (binding.imageView.drawable as BitmapDrawable).bitmap)
            val tmpBitmap = Bitmap.createBitmap(
                    myBitmap.width, myBitmap.height, Bitmap.Config.RGB_565)
            val tmpCanvas = Canvas(tmpBitmap)
            tmpCanvas.drawBitmap(myBitmap, 0F, 0F, null)
            val drawColor = ResourcesCompat.getColor(resources, R.color.green_700, null)
            val paint = Paint(Paint.ANTI_ALIAS_FLAG).apply {
                color = drawColor
                strokeWidth = 5F
                style = Paint.Style.FILL
            }
            tmpCanvas.drawCircle(event.x, event.y, 5F, paint)
            binding.imageView.setImageBitmap(tmpBitmap)
            if (state == State.PICKING_START || state == State.PICKED_START) {
                state = State.PICKED_START
                pickedStartX = event.x
                pickedStartY = event.y
            } else {
                state = State.PICKED_FINISH
                pickedFinishX = event.x
                pickedFinishY = event.y
            }
            binding.doneButton.visibility = View.VISIBLE
            true
        }

        binding.doneButton.setOnClickListener {
            if (state == State.PICKED_START) {
                state = State.PICKING_FINISH
                binding.pointText.setText(R.string.pointText2)
                binding.doneButton.visibility = View.GONE
                binding.imageView.setImageURI(intent.getParcelableExtra("uri"))
            }
//            if (state == State.PICKED_FINISH) {
//                solve(binding.imageView.drawable, pickedStartX, pickedStartY,
//                        pickedFinishX, pickedFinishY)
//            }
        }
    }
}