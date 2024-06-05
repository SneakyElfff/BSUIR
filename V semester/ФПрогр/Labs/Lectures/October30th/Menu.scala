package October30th

import java.awt.event.{ActionEvent, ActionListener}
import java.awt.{Menu, MenuBar, MenuItem}
import javax.swing.{JButton, JFrame, JPanel, SwingUtilities}

object ButtonModule {
  def main(args: Array[String]): Unit = {
    SwingUtilities.invokeLater(() => {
      val frame = new JFrame("My Application")
      frame.setSize(300, 200)

      val menuBar = new MenuBar()
      frame.setMenuBar(menuBar)

      val fileMenu = new Menu("File")
      menuBar.add(fileMenu)

      val exitItem = new MenuItem("Exit")
      exitItem.addActionListener((_:java.awt.event.ActionEvent) => {frame.dispose()})
      fileMenu.add(exitItem)

      val panel = new JPanel()

      val button = new JButton("Click me!")
      panel.add(button)
      button.addActionListener(new ActionListener {
        override def actionPerformed(e: ActionEvent): Unit = {
          println("Hello, world!")
        }
      })

      frame.add(panel)
      frame.setLocationRelativeTo(null)

      frame.setVisible(true)
    })
  }
}