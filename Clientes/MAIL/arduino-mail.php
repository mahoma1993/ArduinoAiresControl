
    <?php
      $mysqli = new mysqli("localhost", "root", "", "tecnologik_system");
      $sql = "SELECT email FROM datos WHERE activo= '1';";
      $result = $mysqli->query($sql);
      $mail='';
      $band = 0;
      while ( $row = $result->fetch_array(MYSQLI_ASSOC) ) 
      {
       if ($band==0){
          $mail=$row['email'];
          $band=1;
       }else {
          $mail=$mail.";".$row['email'];
       }
       
      }

    ?>

<html>
       
       <head>
          <title>Sending HTML email using PHP</title>
       </head>
       
       <body>
     
          <?php
            
             //$to = "tecnologik.soporte08@gmail.com";       //Receiver email.
             $to = $mail; 
             $subject = "ALERTA EN LA SALA DE SERVIDORES";
             $message = $_GET['text'];               //Text variable will be the message content
            	//echo $message;          
             $header = "From: Tecnologik.S.R.L.\r\n";
             $header .= "Cc:\r\n";
             $header .= "MIME-Version: 1.0\r\n";
             $header .= "Content-type: text/html\r\n";
              $retval = mail ($to,$subject,$message,$header);
             //echo $retval;  
             if( $retval == true ) {
                echo "Message sent successfully...";
             }else {
                echo "Message could not be sent...";
             }
             
            
          ?>
          
       </body>
    </html>