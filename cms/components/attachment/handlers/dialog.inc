<?php
Fakoli::using("attachment");

$useCamera = checkNumeric($_GET["use_camera"]);
Fakoli::end(AttachmentFieldRenderer::renderDialog($useCamera));