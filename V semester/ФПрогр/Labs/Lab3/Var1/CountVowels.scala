package Var1

import scala.util.matching.Regex

object Main10 {
  def main(args: Array[String]): Unit = {
    val input = "Oblivious sea I call history"
    println(input)

    val pattern: Regex = "[aeiouAEIOU]".r

    val counter = pattern.findAllIn(input).toList.length

    println("The number of vowels: " + counter)
  }
}