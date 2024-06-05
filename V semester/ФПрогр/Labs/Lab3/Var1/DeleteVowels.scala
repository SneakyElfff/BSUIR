package Var1

import scala.util.matching.Regex

object Main15 {
  def main(args: Array[String]): Unit = {
    val input = "Hello to everybody"
    println(input)

    val pattern: Regex = "[aeiouAEIOU]".r

    val result = pattern.replaceAllIn(input, "")

    println("Without vowels: " + result)
  }
}