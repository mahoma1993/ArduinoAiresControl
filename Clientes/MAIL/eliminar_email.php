<?php 

$id = $_POST['id_email'];
$mysqli = new mysqli("localhost", "root", "", "tecnologik_system");

//preparando mi  consulta para hacer a la base de datos y lograr ver si existe el usuario y contrasenha que se mando desde el navegador
$sql = "DELETE FROM datos WHERE id='$id';";
$result = $mysqli->query($sql);
if ($result > 0) {
header('Location: index.php');
}else{
echo "Error en la eliminacion";
}
 ?>

