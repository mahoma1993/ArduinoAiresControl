<?php 

$nombre= $_POST['nombre'];
$email = $_POST['email'];



$mysqli = new mysqli("localhost", "root", "", "tecnologik_system");

//preparando mi  consulta para hacer a la base de datos y lograr ver si existe el usuario y contrasenha que se mando desde el navegador
$sql = "
INSERT INTO datos (nombre, email)
VALUES ('$nombre','$email');

";
$result = $mysqli->query($sql);
if ($result > 0) {
header('Location: index.php');
}else{
echo "Error en añadir";
}
 ?>