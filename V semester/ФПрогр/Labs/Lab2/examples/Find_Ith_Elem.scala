package examples

object Main8 {

  def posList(lst: List[Int], pos: Int): Int = {
    if (lst.isEmpty) -1
    else if (pos == 0) lst.head
    else {
      val newpos = pos-1
      posList(lst.tail, newpos)}
  }

  def main(args: Array[String]): Unit = {
    val myList = List(1, 2, 3, 4, 5)
    val pos = 3

    val minw = posList(myList, pos)
    println(minw)
  }
}