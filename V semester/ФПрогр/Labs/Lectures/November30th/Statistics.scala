package November30th

/*
19. Аналитические вычисления в Scala. Пример работы со статистикой.
 */

import org.apache.commons.math3.stat.descriptive.{DescriptiveStatistics, SummaryStatistics}

object Main23 {
  def main(args: Array[String]): Unit = {
    val data = Array(1.0, 2.0, 3.0, 4.0, 5.0)

    //calculate mean and standard deviation (отклонение) int the array using DescriptiveStatistics

    //DescriptiveStatistics - класс для работы со статистикой
    val stats1 = new DescriptiveStatistics()    //создает объект для работы
    data.foreach(stats1.addValue)    //добавляет элементы в статистику
    val mean = stats1.getMean()
    val stdDev = stats1.getStandardDeviation()    //стандартное отклонение - корень из дисперсии

    //calculate mean and standard deviation using SummaryStatistics

    val stats2 = new SummaryStatistics()
    data.foreach(stats2.addValue)
    val mean2 = stats2.getMean()
    val stdDev2 = stats2.getStandardDeviation()

    println(s"Mean value: ${mean}")
    println(s"Stdev: ${stdDev}")
    println("good")
  }
}

/*output:
Mean value: 3.0
Stdev: 1.5811388300841898
good
 */