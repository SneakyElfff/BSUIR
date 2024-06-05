package examples

object Main3 {

  def main(args: Array[String]): Unit = {
    val myList = List(1, 2, 3, 4, 5)

    val sum = myList.foldLeft(0)((ac_c, x) => ac_c + x)    //foldLeft(два аргумента): 1 арг - 0 (ас_с = 0), 2 арг - функция

    println(sum)
  }
}