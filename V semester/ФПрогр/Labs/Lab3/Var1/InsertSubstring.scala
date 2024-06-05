package Var1

import scala.util.matching.Regex

object Main18_a {
  def main(args: Array[String]): Unit = {
    val input = "Hello to everybody"

    val pattern: Regex = "Hello".r

    val result = pattern.replaceFirstIn(input, "$0 with heartiness")

    println(result)
  }
}

object Main18_b {
  def main(args: Array[String]): Unit = {
    val input = "Hello to everybody"
    val substr = "with heartiness"

    val pattern: Regex = "Hello".r

    val matched = pattern.findAllMatchIn(input)

    val result = matched.foldLeft(input)((str, matched_data) => {
      val start = matched_data.start
      val end = matched_data.end

      str.substring(0, end+1) + substr + str.substring(end)
    })

    println(result)
  }
}
