package Var1

import scala.util.matching.Regex

object Main13 {
  def main(args: Array[String]): Unit = {
    val input = "A big round ball fall to the ground"

    val pattern: Regex = "A".r

    val result = pattern.replaceAllIn(input, "The")

    println(result)
  }
}