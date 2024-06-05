package examples

object Main5 {
  def main(args: Array[String]): Unit = {
    val str = "Hello, World!"
    val arr = str.toCharArray()

    println(arr.mkString(" "))
  }
}