package Var1

import scala.util.matching.Regex

object Main14 {
  def main(args: Array[String]): Unit = {
    val input = "Get off of my cloud"

    val pattern: Regex = "\\b\\w+\\b".r

    val words = pattern.findAllIn(input).toList

    val reversed = words.foldLeft(List.empty[String])((acc, word) => word :: acc)

    println(reversed.mkString(" "))
  }
}