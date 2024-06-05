package Var1

import scala.util.matching.Regex

object Main17_a {
  def main(args: Array[String]): Unit = {
    val input = "Hello to everybody"
    println(input)

    val pattern: Regex = "y|Y".r

    val result = pattern.replaceAllIn(input, "")

    println("Without y's: " + result)
  }
}

object Main17_b {
  def main(args: Array[String]): Unit = {
    val input = "Hello to everybody"
    println(input)

    val pattern: Regex = "y|Y".r

    val matched = pattern.findAllMatchIn(input)

    var offset = 0

    val result = matched.foldLeft(input)((str, matched_data) => {
      val start = matched_data.start
      val end = matched_data.end
      val temp = offset

      offset += end - start

      str.substring(0, start-temp) + str.substring(end-temp)
    })

    println("Without y's: " + result)
  }
}
