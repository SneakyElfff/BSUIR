package Var1

import scala.util.matching.Regex

object Main12 {
  def main(args: Array[String]): Unit = {
    val input = "When executing the exercise extract all extra words"

    val pattern: Regex = "\\bext\\w*\\b".r

    val result = pattern.findAllIn(input).toList

    println(result.mkString(", "))
  }
}