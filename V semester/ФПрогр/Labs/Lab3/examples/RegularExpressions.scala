package examples

object Main {
  def main(args: Array[String]): Unit = {
    val regex = """(\d{3})-(\d{2})-(\d{4})""".r
    val str1 = "123-45-6789"
    val str2 = "abc-12-3456"

    val match1 = regex.findFirstMatchIn(str1)    //возвращает объект Option[Match] - первое совпадение шаблона в строке
    val match2 = regex.findFirstMatchIn(str2)

    match1 match {
      case Some(m) => println(s"Match found: ${m.group(0)}")
      case None => println("No match found")
    }

    match2 match {
      case Some(m) => println(s"Match found: ${m.group(0)}")
      case None => println("No match found")
    }

    val str = "The quick brown fox jumps over the lazy dog"
    val pattern = "fox".r
    val matches = pattern.findAllIn(str)

    matches.foreach(println)
  }
}