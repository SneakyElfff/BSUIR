package Var1

import scala.util.matching.Regex

object Main11 {
  val latinToCyrillicMap = Map(
    "a" -> "а",
    "b" -> "б",
    "c" -> "ц",
    "d" -> "д",
    "e" -> "е",
    "f" -> "ф",
    "g" -> "г",
    "h" -> "х",
    "i" -> "и",
    "j" -> "дж",
    "k" -> "к",
    "l" -> "л",
    "m" -> "м",
    "n" -> "н",
    "o" -> "о",
    "p" -> "п",
    "q" -> "к",
    "r" -> "р",
    "s" -> "с",
    "t" -> "т",
    "u" -> "ю",
    "v" -> "в",
    "w" -> "уэ",
    "x" -> "кс",
    "y" -> "и",
    "z" -> "з"
  )

  def convertLatinToCyrillic(letter: String): String = {
    val cyrillic = latinToCyrillicMap.getOrElse(letter.toLowerCase, letter)

    if (letter(0).isUpper) cyrillic.toUpperCase
    else cyrillic
  }

  def main(args: Array[String]): Unit = {
    val input = "Hello to everybody"
    println("In latin: " + input)

    val pattern: Regex = "[a-zA-Z]".r

    val converted = pattern.replaceAllIn(input, letter => convertLatinToCyrillic(letter.matched))

    println("In cyrillic: " + converted)
  }
}