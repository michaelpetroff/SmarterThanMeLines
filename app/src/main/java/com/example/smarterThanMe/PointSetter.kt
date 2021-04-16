package com.example.smarterThanMe

import android.graphics.Bitmap
import android.os.Bundle
import androidx.appcompat.app.AppCompatActivity
import kotlinx.android.synthetic.main.activity_point_setter.*


class PointSetter : AppCompatActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_point_setter)

        imageView.setImageURI(intent.getParcelableExtra("uri"))
    }
}