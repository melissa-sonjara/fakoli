<?php
Fakoli::using("template");
Fakoli::assertRole("admin");

$file = sanitizePath($_REQUEST["file"]);

$path = $config['homedir'] . DIRECTORY_SEPARATOR . $file;

if ($method == "POST")
{
	$content = $_POST["content"];
	
	$fp = fopen($path, "w");
	fputs($fp, $content);
	fclose($fp);
	echo "1";
}
else
{
	$content = htmlsafe(file_get_contents($path));
	
	$readonly = !is_writable($path);
		
	?>
	<form method="POST" action="/action/template/edit" id="edit_form">
	<input type="hidden" name="file" value="<?echo htmlsafe($file)?>"/>
	<label>
	File Contents<? if ($readonly) echo "&nbsp;<em>(Read Only)</em>";?>
	</label><br/>
	<textarea id="content" name="content" style="width: 650px; height: 350px; font-size: 10px<? if ($readonly) echo '; read-only: true';?>"><?echo $content?></textarea>
	<br/>
	<br/>
	<input type="submit" value=" Save Changes " class="button"/>	
	</form>
	<script type='text/javascript'>
	$('content').resizable();
	$('edit_form').iFrameFormRequest({'onComplete': editResult, 'onFailure': function() { alert("Failed to communicate with server"); }});
	</script>
<?
}
?>