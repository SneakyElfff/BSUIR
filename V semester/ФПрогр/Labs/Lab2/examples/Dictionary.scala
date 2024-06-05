package examples

object Main4 {
  def main(args: Array[String]): Unit = {
    val a = List(1, 2, 3)
    val b = List("one", "two", "three")

    val zipped = a.zip(b)
    println(zipped)

    val filteredList = zipped.filter { case (a, _) => a == 1 }    //например, (1, "one"): a - 1, _ - "one"
    println(filteredList)

    val tup = filteredList.head
    println(tup)

    val secondItem = tup._2
    println(secondItem)
  }
}