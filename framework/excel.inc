<?php
/**
 * Generate a binary format Microsoft Excel file for download.
 * @author andy
 */
class ExcelFile
{
	var $filename;
	var $open;
	var $content;
	
	/**
	 * Create a new ExcelFile object
	 * @param string $filename the name of the file to generate
	 */
	function ExcelFile($filename)
	{
		$this->filename = $filename;
		ob_start();
		$this->open = true;
		$this->_xlsBOF();
	}
	
	
	function _xlsBOF() 
	{
    	echo pack("ssssss", 0x809, 0x8, 0x0, 0x10, 0x0, 0x0);  
	}

	function _xlsEOF() 
	{
    	echo pack("ss", 0x0A, 0x00);
	}

	/**
	 * Write a number to the cell at the specified row and column
	 * @param integer $row
	 * @param integer $col
	 * @param number $value
	 */
	function writeNumber($row, $col, $value) 
	{
    	echo pack("sssss", 0x203, 14, $row, $col, 0x0);
    	echo pack("d", $value);
	}

	/**
	 * Write text to the cell at the specified row and column 
	 * @param integer $row the cell row number
	 * @param integer $col the cell column number
	 * @param string $value the string to insert
	 */
	function writeText($row, $col, $value ) 
	{
    	$len = strlen($value);
    	echo pack("ssssss", 0x204, 8 + $len, $row, $col, 0x0, $len);
    	echo $value;
	}
	
	function abort($error)
	{
		ob_end_clean();
		die($error);
	}
	
	function close()
	{
		$this->_xlsEOF();
		$this->content = ob_get_contents();
		ob_end_clean();
		$this->open = false;
	}
	
	function send()
	{
		if ($this->open)
		{
			$this->close();
		}

		header("Content-Type: application/vnd.msexcel");
		header("Content-Disposition: attachment;filename=$this->filename");
		header("Content-Transfer-Encoding: binary");
		header("Content-Length: ".strlen($this->content));
		
		echo $this->content;
	}
} 