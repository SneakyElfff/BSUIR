package examples

object Main1 {
  def main(args: Array[String]): Unit = {
    val str = "Hello, World!"
    val newStr = str.replaceAll("o", "a")

    println(newStr)
  }
}