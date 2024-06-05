package November16th

/*
17. Пример чения-записи в файл.
 */

import java.io.PrintWriter
import scala.io.Source

object FileInputOutputExample {
  def main(args: Array[String]): Unit = {
    val filename = "example.txt"
    val file = Source.fromFile(filename)
    for (line <- file.getLines) {
      println(line)
    }

    file.close()

    val outputFilename = "output.txt"
    val writer = new PrintWriter(outputFilename)
    writer.write("Hello, world!\n")
    writer.write("This is an example of writing to a file.")
    writer.close()
  }
}