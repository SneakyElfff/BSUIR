package December7th

/*
25. Scala-приложение для распознавания картинок. Программа для определения статистических параметров картинки.
 */

import org.apache.commons.math3.stat.descriptive.DescriptiveStatistics
import java.awt.image.BufferedImage
import java.io.File
import java.util.Random
import javax.imageio.ImageIO

/*Случайным образом выбираем 1000 пикселей.
Определяем среднее значение пикселя и стандратное отклонение.
Эти два параметра будут характеризовать картинку
 */

object Main30 {
  def main(args: Array[String]): Unit = {
    val imagePath = "RightImage.jpg"

    val image: BufferedImage = ImageIO.read(new File(imagePath))

    val width = image.getWidth
    val height = image.getHeight

    val pixels = Array.ofDim[Int](width * height)
    image.getRGB(0, 0, width, height, pixels, 0, width) //заполняется массив пикселей значениями пикселей изображения

    //случайным образом отбираем 1000 пикселей
    val count = 1000
    var numbers = List[Double]()    //хранятся значения пикселей, выбранных случайно
    val random = new Random()

    for (i <- 0 until count) {
      var j = random.nextInt(width * height)
      numbers = math.abs(pixels(j)) :: numbers
    }

    //находим среднее значение и стандартное отклонение
    val stats1 = new DescriptiveStatistics()
    numbers.foreach(stats1.addValue)
    val mean = stats1.getMean()
    val stdDev = math.sqrt(stats1.getStandardDeviation())

    println(s"Mean value: ${mean}")
    println(s"Stdev: ${stdDev}")
  }
  }
