<?php
/**************************************************************

 Copyright (c) 2007-2010 Sonjara, Inc

 Permission is hereby granted, free of charge, to any person
 obtaining a copy of this software and associated documentation
 files (the "Software"), to deal in the Software without
 restriction, including without limitation the rights to use,
 copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the
 Software is furnished to do so, subject to the following
 conditions:

 The above copyright notice and this permission notice shall be
 included in all copies or substantial portions of the Software.

 Except as contained in this notice, the name(s) of the above 
 copyright holders shall not be used in advertising or otherwise 
 to promote the sale, use or other dealings in this Software 
 without prior written authorization.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 OTHER DEALINGS IN THE SOFTWARE.

*****************************************************************/

/**
 * ExcelFileWriter is an abstract factory to allow different 
 * Excel libraries to be used at the programmer's preference.
 * 
 * @author andy
 *
 */
class ExcelFileWriter
{
	static $handler = ExcelFile;
	
	static function create($filename)
	{
		return new ExcelFileWriter::$handler($filename);
	}
	
	static function RegisterHandler($handlerClass)
	{
		ExcelFileWriter::$handler = $handlerClass;
	}
}

/**
 * Minimum base interface that must be provided by an ExcelFile implementation.
 * @author andy
 *
 */
interface IExcelFile
{
	/**
	 * Write a number to the cell at the specified row and column
	 * @param integer $row
	 * @param integer $col
	 * @param number $value
	 */
	function writeNumber($row, $col, $value);
	
	/**
	 * Write text to the cell at the specified row and column 
	 * @param integer $row the cell row number
	 * @param integer $col the cell column number
	 * @param string $value the string to insert
	 */
	function writeText($row, $col, $value );
	
	/**
	 * Abort processing, output the specified error string
	 * @param string $error
	 */
	function abort($error);

	/**
	 * Prevent further output to the spreadsheet.
	 */	
	function close();
	
	/**
	 * Transmit the spreadsheet to the client.
	 */
	function send();
}

/**
 * Generate a binary format Microsoft Excel file for download.
 * The default ExcelFile writer is an extremely limited implementation
 * that outputs BIFF8 format (Excel 97/2000). It is designed to be 
 * compact and provide Excel output without incurring any external 
 * dependencies.
 * @author andy
 */
class ExcelFile implements IExcelFile
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
		ajaxReturn($error);
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

		session_cache_limiter("private_no_expire, must-revalidate");
		header("Content-Type: application/vnd.msexcel");
		header("Content-Disposition: attachment;filename=$this->filename");
		header("Pragma: private");
		header("Content-Transfer-Encoding: binary");
		header("Content-Length: ".strlen($this->content));
		
		echo $this->content;
	}
} 