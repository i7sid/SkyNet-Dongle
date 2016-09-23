<?php
/**
 * IGC_J_Record class extends IGC_Record. The J record  defines the extension K Record.
 * @see IGC_Record
 *
 * @version 0.1
 * @author Mike Milano <coder1@gmail.com>
 * @project php-igc
 */
class IGC_J_Record extends IGC_Record
{
  /**
   * Start byte number
   *
   * @access public
   * @var integer
   */
  public $start_byte_number;

  /**
   * Finish byte number
   *
   * @access public
   * @var integer
   */
  public $finish_byte_number;

  /**
   * Mnemonic
   *
   * @access public
   * @var string
   */
  public $mnemonic;

  /**
   * Class constructor creates the J record from the raw IGC string
   *
   * @param     string  $record
   */
  public function __construct($record)
  {
    $this->type = 'J';
    $this->raw = $string;

    $this->start_byte_number = substr($record,1,2);
    $this->finish_byte_number = substr($record,3,2);
    $this->mnemonic = substr($record,5,3);
  }
}
?>
