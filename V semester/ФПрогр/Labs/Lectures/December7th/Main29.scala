package December7th

/*
24. Scala-приложение для распознавания картинок. Программа для чтения массива пикселей.
 */

import java.awt.image.BufferedImage
import java.io.File
import javax.imageio.ImageIO

object Main29 {
  def main(args: Array[String]): Unit = {
    val imagePath = "RightImage.jpg"

    val image: BufferedImage = ImageIO.read(new File(imagePath))    //

    val width = image.getWidth
    val height = image.getHeight

    val pixels = Array.ofDim[Int](width * height)
    image.getRGB(0, 0, width, height, pixels, 0, width)    //заполняется массив пикселей значениями пикселей изображения

    for (i <- 0 until 10) {
      println(pixels(i))
    }
  }
  }