<?php
Fakoli::using("attachment");

$useCamera = checkNumeric($_GET["use_camera"]);

$attachment = new Attachment();

$form = new AutoForm($attachment, "POST", "/action/attachment/attachment_form?use_camera={$useCamera}");
$form->onSavePreProcess = function($form)
{
    global $user;
    if ($user)
    {
        $form->data->owner_id = $user->getPrimaryKeyValue();
    }
};

$fileUpload = new FileUploadFieldRenderer($form, "attachmentFile", "Select File", function($field, $attachment) use ($form)
{
    global $config, $user;

    // Clear any previous filter
    $attachment->filter = null;

    if (!$_FILES[$field]) 
    {
        $form->msg = "No upload record found.";
        return false;
    }

    if ($_FILES[$field]["name"]=="") 
    {
        $form->msg = "Upload name is empty";
    }

    /* Copy across the uploaded file */

    trace("Upload Base: {$config['uploadbase']}", 3);
    trace("Upload Directory: {$config['uploaddir']}", 3);

    $dir = $config["attachmentdir"];
    $name = $_FILES[$field]["name"];
    $base = $config['uploadbase'];

    trace ("Upload name _FILES $field name $name", 3);
    trace ("Upload attachment dir $dir", 3);
    trace ("Uploading file to $base/$file", 3);

    if (!file_exists("$base/$dir"))
    {
        // If the directory does not exist, create it 
        mkdir("{$config['uploadbase']}/$dir");
    }

    $attachment->user_id = $user->get($user->primary_key);
    $attachment->save();

    $extension = substr($name, strrpos($name, "."));

    $file = "$dir/{$attachment->attachment_id}$extension";

    trace ("Uploading file dir/attachment id extension: $file", 3);

    if (file_exists("$base/$file"))
    {
        // If a previous copy of the file already exists, remove it
        unlink("$base/$file");
    }

    move_uploaded_file($_FILES['attachmentFile']["tmp_name"], "$base/$file");
    chmod("$base/$file", 0755);
    
    $size =  getScaledSize(filesize("$base/$file"));

    $attachment->filename = $name;
    $attachment->local_filename = $file;
    $attachment->file_size =$size;
    $attachment->save();

    return true;
});

$form->hideExcept("attachmentFile");

$form->ajaxSubmit("function(result) {AttachmentManager.activeManager.fileUploaded(JSON.parse(result));}", "function() {document.id('{$form->id}_error').set('text','Failed to communicate with server'); }");

if ($method == "POST")
{
    if ($form->save())
    {
        $icon = DocumentHandler::getDocIcon($attachment->filename);
        Fakoli::JSONreturn((object)["success" => true, "attachment_id" => $attachment->attachment_id, "name" => $attachment->filename, "size" => $attachment->file_size, "icon" => $icon], false, false);
    }
    else
    {
        Fakoli::JSONreturn((object)["success" => false, "error" => $form->msg], false, false);
    }
}

echo $form->writeScript();

?>
   <p>Select a file from your local device.</p>
<?
$form->drawForm();
?>