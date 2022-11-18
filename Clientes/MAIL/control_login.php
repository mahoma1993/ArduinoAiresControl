<?php 
$user = $_POST['user'];
$pass = $_POST['pass'];

/*
mysqli_connect funcion propia de php que sirve para comunicarme con mysql, 
los parametros son
localhost: donde esta ejecutandose el mysql, en mi caso, localmente
root: usuario para la base de datos, en este caso root
password: la contrasenha que tiene ese ususario (root) para acceder a la bd
base de datos: (tecnologik_system) el nombre que tiene tu base de datos, que es el origen total de la info
*/
$mysqli = new mysqli("localhost", "root", "", "tecnologik_system");

//preparando mi  consulta para hacer a la base de datos y lograr ver si existe el usuario y contrasenha que se mando desde el navegador
$sql = "SELECT * FROM usuarios WHERE USER = '$user' AND pass='$pass';";
$result = $mysqli->query($sql);
$row_cnt = $result->num_rows;

if ($row_cnt > 0) {
	session_start();
	$_SESSION['logeado'] = 1;
header('Location: index.php');
}else{
header('Location: login.php');
}




 ?>