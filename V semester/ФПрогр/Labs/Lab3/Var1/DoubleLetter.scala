package Var1

import scala.util.matching.Regex

object Main16 {
  def main(args: Array[String]): Unit = {
    val input = "Hello to everybody"

    val pattern: Regex = "[A-Za-z]".r

    val result = pattern.replaceAllIn(input, letter => letter.matched + letter.matched)

    println(result)
  }
}