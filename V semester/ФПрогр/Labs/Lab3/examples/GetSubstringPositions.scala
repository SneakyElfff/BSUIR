package examples

object Main8 {
  def main(args: Array[String]): Unit = {
    val str = "Hello, World!"

    println(str.indexOf("o"))
    println(str.lastIndexOf("o"))

    println(str.indexOf("World"))
    println(str.lastIndexOf("World"))
  }
}