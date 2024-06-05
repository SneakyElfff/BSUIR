package October6th

import java.io.{BufferedInputStream, DataInputStream, File, FileInputStream, IOException}

class Finalize {
  val fileHandle = new File("data.txt")
  val fis = new FileInputStream(fileHandle)    //поток ввода
  val bis = new BufferedInputStream(fis)    //читаем блок в память
  val dis = new DataInputStream(bis)    //чтение данных

  try {
    while (dis.available() != 0) {
      val line = dis.readLine()
      println(line)
    }
  } catch {
    case e: IOException =>
      println("An error occurred while reading: " + e.getMessage)
  } finally {
    dis.close()
    println("\nreading closed")
  }

  override def finalize(): Unit = {
    super.finalize()    //освободить ресурсы
  }
}

object Main {
  def main(args: Array[String]): Unit = {
    val myObject2 = new Finalize
    myObject2.finalize()
    println("\nok")
  }
}