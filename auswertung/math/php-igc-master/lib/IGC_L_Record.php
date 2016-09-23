<?php
/**
 * IGC_L_Record class extends IGC_Record. Log Book
 *
 * @see IGC_Record
 *
 * @version 0.1
 * @author Mike Milano <coder1@gmail.com>
 * @project php-igc
 */
class IGC_L_Record extends IGC_Record
{
  /**
   * Manufacturer
   *
   * @access public
   * @var string
   */
  public $manufacturer;

  /**
   * Comment
   *
   * @access public
   * @var string
   */
  public $comment;

  /**
   * Class constructor creates the L record from the raw IGC string
   *
   * @param     string  $record
   */
  public function __construct($string)
  {
    $this->type = 'L';
    $this->raw = $string;

    $this->manufacturer = substr($string,1,1);
    $this->comment = substr($string,2);
  }
}
?>
