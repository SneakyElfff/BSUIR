package examples

object Main {
  def double(x: Int): Int = x * 2

  def main(args: Array[String]): Unit = {
    val myList = List(1, 2, 3, 4, 5)

    val doubledList = myList.map(double)    //к каждому элементу списка применяется функция

    println(doubledList)
  }
}