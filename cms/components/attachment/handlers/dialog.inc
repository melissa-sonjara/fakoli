<?php
Fakoli::using("attachment");

$useCamera = checkNumeric($_GET["use_camera"]);
echo AttachmentFieldRenderer::renderDialogBody($useCamera);