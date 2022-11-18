<?php 
session_start();
if (!isset($_SESSION['logeado'])) 
{
  header('Location: login.php');
}
 ?>
    
<!DOCTYPE html>
<html>
<head>
  <title></title>
<link rel="stylesheet" href="https://stackpath.bootstrapcdn.com/bootstrap/4.4.1/css/bootstrap.min.css" integrity="sha384-Vkoo8x4CGsO3+Hhxv8T/Q5PaXtkKtu6ug5TOeNV6gBiFeWPGFN9MuhOf23Q9Ifjh" crossorigin="anonymous">
</head>
<body>

<div class="container">
<div class="card">
  <div class="card-header">
    Correos Para envio de mail
  </div>
  <div class="card-body">

<table class="table">
  <thead class="thead-dark">
    <tr>
      <th scope="col">nombre</th>
      <th scope="col">email</th>
      <th scope="col">Eliminar</th>
    </tr>
  </thead>
  <tbody>

    <?php
      $mysqli = new mysqli("localhost", "root", "", "tecnologik_system");
      $sql = "SELECT * FROM datos WHERE activo= '1';";
      $result = $mysqli->query($sql);
      while ( $row = $result->fetch_array(MYSQLI_ASSOC) ) 
      {
        echo '<form action="eliminar_email.php" method="POST">';
       echo '<tr>';
       echo '<td>'.$row['nombre'].'</td>';
       echo '<td>'.$row['email'].'</td>';
       echo '<td><button class="btn btn-danger btn-sm" name="id_email" value="'.$row['id'].'">X</button></td>';
      echo '</tr>';
      echo '</form>';
       
      }
    ?>

      </tbody>
</table>

    <button type="button" class="btn btn-primary" data-toggle="modal" data-target="#exampleModal">
  a&ntilde;adir
</button>
  </div>
</div>
</div>

<!-- Modal -->
<div class="modal fade" id="exampleModal" tabindex="-1" role="dialog" aria-labelledby="exampleModalLabel" aria-hidden="true">
  <div class="modal-dialog" role="document">
    <div class="modal-content">
      <div class="modal-header">
        <h5 class="modal-title" id="exampleModalLabel">A&ntilde;adir Email</h5>
        <button type="button" class="close" data-dismiss="modal" aria-label="Close">
          <span aria-hidden="true">&times;</span>
        </button>
      </div>
      <div class="modal-body">


        <form action="guardar_mail.php" method="POST">
          <div class="form-group">
            <label for="formGroupExampleInput">Nombre</label>
            <input type="text" class="form-control" id="formGroupExampleInput" name="nombre" placeholder="" required="">
          </div>
          <div class="form-group">
            <label for="formGroupExampleInput2">Email</label>
            <input type="email" class="form-control" id="formGroupExampleInput2" name="email" placeholder="" required="">

          </div>
           <button type="button" class="btn btn-secondary" data-dismiss="modal">Cerrar</button>
          <button type="submit" class="btn btn-primary">Guardar</button>
        </form>





      </div>
      <div class="modal-footer">
       
      </div>



    </div>
  </div>
</div>


</body>
<script src="https://code.jquery.com/jquery-3.4.1.slim.min.js" integrity="sha384-J6qa4849blE2+poT4WnyKhv5vZF5SrPo0iEjwBvKU7imGFAV0wwj1yYfoRSJoZ+n" crossorigin="anonymous"></script>
<script src="https://cdn.jsdelivr.net/npm/popper.js@1.16.0/dist/umd/popper.min.js" integrity="sha384-Q6E9RHvbIyZFJoft+2mJbHaEWldlvI9IOYy5n3zV9zzTtmI3UksdQRVvoxMfooAo" crossorigin="anonymous"></script>
<script src="https://stackpath.bootstrapcdn.com/bootstrap/4.4.1/js/bootstrap.min.js" integrity="sha384-wfSDF2E50Y2D1uUdj0O3uMBJnjuUD4Ih7YwaYd1iqfktj0Uod8GCExl3Og8ifwB6" crossorigin="anonymous"></script>
</html>
