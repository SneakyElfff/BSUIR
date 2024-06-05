import java.awt.event.{ActionEvent, ActionListener}
import javax.swing.{JButton, JFrame, JPanel, SwingUtilities}
import java.awt.Dimension
import javax.swing.{JFrame, JLabel, JTextField, JTextArea, JPanel, SwingUtilities}
import java.sql.{Connection, DriverManager, ResultSet}
import java.awt.Color
import scala.util.Random

object ButtonModule {
  val frame = new JFrame("My Application")
  frame.setSize(800, 600)
  frame.setLayout(null)
  
  frame.getContentPane().setBackground(java.awt.Color.CYAN)    //получить доступ к контейнеру содержимого JFrame
  
  def main(args: Array[String]): Unit = {
    SwingUtilities.invokeLater(() => {
    val label_name = new JLabel("Title")
    val label_price = new JLabel("Price")
    val label_quant = new JLabel("Quantity")
    label_name.setBounds(70,120,100,20)
    label_price.setBounds(207,120,70,20)
    label_quant.setBounds(340,120,70,20)
    
    val insert = new JButton("Insert")
    val select = new JButton("Select")
    val get_max = new JButton("Max profit")
    val find = new JButton("Find")
    val group_by = new JButton("Group by")
    val match_button = new JButton("Match")
    insert.setBounds(20,80,100,20)
    select.setBounds(130,80,100,20)
    get_max.setBounds(240,80,100,20)
    find.setBounds(350,80,100,20)
    group_by.setBounds(460,80,100,20)
    match_button.setBounds(570,80,100,20)
    
    insert.setBackground(Color.WHITE)
    insert.setForeground(Color.BLUE)
    select.setBackground(Color.WHITE)
    select.setForeground(Color.BLUE)
    get_max.setBackground(Color.WHITE)
    get_max.setForeground(Color.BLUE)
    find.setBackground(Color.WHITE)
    find.setForeground(Color.BLUE)
    group_by.setBackground(Color.WHITE)
    group_by.setForeground(Color.BLUE)
    match_button.setBackground(Color.WHITE)
    match_button.setForeground(Color.BLUE)
    
    val textField = new JTextArea(10,20)
    val textField2 = new JTextArea(10,20)
    val textField3 = new JTextArea(10,20)
    textField.setBounds(20,150,130,200)
    textField2.setBounds(160,150,130,200)
    textField3.setBounds(300,150,130,200)
    
    textField.setLineWrap(true)    //включение автопереноса строк
    textField.setWrapStyleWord(true)
    
    textField2.setLineWrap(true)    //включение автопереноса строк
    textField2.setWrapStyleWord(true)
    
    frame.add(label_name)
    frame.add(label_price)
    frame.add(label_quant)
    
    frame.add(insert)
    frame.add(select)
    frame.add(get_max)
    frame.add(find)
    frame.add(group_by)
    frame.add(match_button)
    
    frame.add(textField)
    frame.add(textField2)
    frame.add(textField3)
    
    insert.addActionListener(new ActionListener {
      override def actionPerformed(e: ActionEvent): Unit = {
        val url = "jdbc:mysql://localhost:3306/mydb"
        val username = "root"
        val password = "nina"
    
        Class.forName("com.mysql.jdbc.Driver")    //класс драйвера для работы приложения с базой данных
        val conn = DriverManager.getConnection(url, username, password)    //канал для отправки запросов
        try {
          val stmt = conn.createStatement()    //объект для создания запросов (select, insert, update, delete)
    
          val prod_name = textField.getText().trim()
          val price = textField2.getText().trim()
          val quantity = textField3.getText().trim()
          val random = new Random()
          val id: Int = random.nextInt()
    
          val res_check = stmt.executeQuery(s"SELECT product FROM warehouse WHERE product = '$prod_name'")
    
          if (res_check.next()) {
            textField.setText("Value already exists")
            
            val res = stmt.executeUpdate(s"INSERT INTO warehouse VALUES ('$prod_name', '$price', '$quantity', '$id')")
    
            if (res > 0) {
              textField2.setText("")
              textField3.setText("")
              textField.setText("Added record")
            } else {
              textField.setText("Failed to add record")
            }
            
          } else {
            val res = stmt.executeUpdate(s"INSERT INTO warehouse VALUES ('$prod_name', '$price', '$quantity', '$id')")
    
            if (res > 0) {
              textField2.setText("")
              textField3.setText("")
              textField.setText("Added record")
            } else {
              textField.setText("Failed to add record")
            }
          }
        } finally {    //в любом случае сработает
          conn.close()
        }
      }
    })
    
    select.addActionListener(new ActionListener {
      override def actionPerformed(e: ActionEvent): Unit = {
      {
        val url = "jdbc:mysql://localhost:3306/mydb"
        val username = "root"
        val password = "nina"
        
        Class.forName("com.mysql.jdbc.Driver")
        val conn = DriverManager.getConnection(url, username, password)
        try {
          val stmt = conn.createStatement()
          
          val prod_name= textField.getText().toString().trim()
          
          val rs = stmt.executeQuery("SELECT * FROM warehouse WHERE product = '"+prod_name+"'")
          
          while (rs.next()) {
            val name = rs.getString("product")
            val price = rs.getInt("price")
            val quantity = rs.getInt("quantity")
            
            textField2.setText("" + price)
            textField3.setText("" + quantity)
          }
        } finally {
            conn.close()
        }
      }
      }
    })
    
    get_max.addActionListener(new ActionListener {
      override def actionPerformed(e: ActionEvent): Unit = {
      {
        val url = "jdbc:mysql://localhost:3306/mydb"
        val username = "root"
        val password = "nina"
        
        Class.forName("com.mysql.jdbc.Driver")
        val conn = DriverManager.getConnection(url, username, password)
        try {
          val stmt = conn.createStatement()
          
//          val rs = stmt.executeQuery("SELECT product FROM warehouse WHERE (price * quantity) = (SELECT MAX(price * quantity) FROM warehouse)")
          val rs = stmt.executeQuery("SELECT product " +
                  "FROM warehouse " +
                  "WHERE quantity IS NOT NULL AND quantity > 0 " +
                  "ORDER BY price * quantity DESC " +
                  "LIMIT 1")

          if (rs.next()) {
            val prod_name = rs.getString("product")
            textField2.setText("")
            textField3.setText("")
            textField.setText(prod_name)
          } else {
            textField.setText("No data found")
          }
        } finally {
            conn.close()
        }
      }
      }
    })
    
    find.addActionListener(new ActionListener {
      override def actionPerformed(e: ActionEvent): Unit = {
      {
        val url = "jdbc:mysql://localhost:3306/mydb"
        val username = "root"
        val password = "nina"
        
        Class.forName("com.mysql.jdbc.Driver")
        val conn = DriverManager.getConnection(url, username, password)
        try {
          val stmt = conn.createStatement()
          
          val price_cond = textField2.getText().toString().trim()
          val quant_cond = textField3.getText().toString().trim()
          
          val query =
            if (price_cond.nonEmpty && quant_cond.nonEmpty) {
              s"SELECT product FROM warehouse WHERE price >= $price_cond AND quantity >= $quant_cond"
            } else if (price_cond.nonEmpty) {
              s"SELECT product FROM warehouse WHERE price >= $price_cond"
            } else if (quant_cond.nonEmpty) {
              s"SELECT product FROM warehouse WHERE quantity >= $quant_cond"
            } else {
              "SELECT product FROM warehouse"
            }
          
          val rs = stmt.executeQuery(query)
          
          if (rs.next()) {
            val prod_name = rs.getString("product")
            textField.setText(prod_name)
          } else {
            textField.setText("No data found")
          }
        } finally {
            conn.close()
        }
      }
      }
    })
    
    group_by.addActionListener(new ActionListener {
      override def actionPerformed(e: ActionEvent): Unit = {
      {
        val url = "jdbc:mysql://localhost:3306/mydb"
        val username = "root"
        val password = "nina"
        
        Class.forName("com.mysql.jdbc.Driver")
        val conn = DriverManager.getConnection(url, username, password)
        try {
          val stmt = conn.createStatement()
          
          val rs = stmt.executeQuery("SELECT product, AVG(price) AS average_price FROM warehouse GROUP BY product")
          
          while (rs.next()) {
            val prod_name = rs.getString("product");
            textField.setText(textField.getText() + "\n" + prod_name)
            val avg_price = rs.getDouble("average_price");
            textField2.setText(textField2.getText() + "\n" + avg_price)
          } 
        } finally {
            conn.close()
        }
        }
      }
    })
    
    match_button.addActionListener(new ActionListener {
      override def actionPerformed(e: ActionEvent): Unit = {
      {
        val url = "jdbc:mysql://localhost:3306/mydb"
        val username = "root"
        val password = "nina"
        
        Class.forName("com.mysql.jdbc.Driver")
        val conn = DriverManager.getConnection(url, username, password)
        try {
          val stmt = conn.createStatement()
          
          val prod_name = textField.getText().toString().trim()
          
          val rs = stmt.executeQuery(s"SELECT * FROM warehouse WHERE product LIKE '%$prod_name%'")
          
          while (rs.next()) {
            val name = rs.getString("product")
            val price = rs.getInt("price")
            val quantity = rs.getInt("quantity")
            
            textField.setText(textField.getText() + "\n" + prod_name)
            textField2.setText(textField2.getText() + "\n" + price)
            textField3.setText(textField3.getText() + "\n" + quantity)
            }
        } finally {
            conn.close()
        }
        }
      }
    })
    
    frame.setBackground(Color.BLUE)
    frame.setLocationRelativeTo(null)
    frame.setVisible(true)
    })
  }
}