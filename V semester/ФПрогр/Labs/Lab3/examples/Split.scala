package examples

object Main2 {
  def main(args: Array[String]): Unit = {
    val str = "apple,banana,orange"
    val arr = str.split(",")

    println(arr.mkString(" "))
  }
}