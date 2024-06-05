package December7th

/*
26. Scala-приложение для распознавания картинок. Программа для сравнения двух картинок.
 */

import org.apache.commons.math3.stat.descriptive.DescriptiveStatistics
import java.awt.image.BufferedImage
import java.io.File
import java.util.Random
import javax.imageio.ImageIO

/*Ключевая идея: чтобы распознать картинку,
будем сравнивать ее с другой картинкой (в идеале из базы данных, в данном случае - из файла).
Сравниваем среднее значение и стандартное отклонение.
Выбираем картинку по максимальному сходству -
используем для этого косинусную метрику: если косинус близок к 1, то картинки схожи.
Для этого нужно запомнить номера выбранных случайным образом пикселей
 */

object Main31 {
  def main(args: Array[String]): Unit = {
    var imagePath = "RightImage.jpg"

    val image: BufferedImage = ImageIO.read(new File(imagePath))

    val width = image.getWidth
    val height = image.getHeight

    val pixels = Array.ofDim[Int](width * height)
    image.getRGB(0, 0, width, height, pixels, 0, width) //заполняется массив пикселей значениями пикселей изображения

    //случайным образом отбираем 1000 пикселей
    val count = 1000
    var numbers = List[Double]()    //хранятся значения пикселей, выбранных случайно
    var nomers = List[Int]()    //хранит их индексы
    val random = new Random()

    for (i <- 0 until count) {
      var j = random.nextInt(width * height)
      nomers = j :: nomers
      numbers = math.abs(pixels(j)) :: numbers
    }

    //находим среднее значение и стандартное отклонение
    val stats1 = new DescriptiveStatistics()
    numbers.foreach(stats1.addValue)
    val mean = stats1.getMean()
    val stdDev = math.sqrt(stats1.getStandardDeviation())

    println(s"Mean value: ${mean}")
    println(s"Stdev: ${stdDev}")


    imagePath = "LeftImage.jpg"

    val image2: BufferedImage = ImageIO.read(new File(imagePath))

    val pixels2 = Array.ofDim[Int](width * height)
    image2.getRGB(0, 0, width, height, pixels2, 0, width) //заполняется массив пикселей значениями пикселей изображения

    var numbers2 = List[Double]()    //хранятся значения пикселей, выбранных случайно

    for (i <- 0 until count) {
      numbers2 = math.abs(pixels(i)) :: numbers2
    }

    //находим среднее значение и стандартное отклонение
    val stats2 = new DescriptiveStatistics()
    numbers2.foreach(stats2.addValue)
    val mean2 = stats2.getMean()
    val stdDev2 = math.sqrt(stats2.getStandardDeviation())

    println(s"Mean-2 value: ${mean2}")
    println(s"Stdev-2: ${stdDev2}")
  }
}