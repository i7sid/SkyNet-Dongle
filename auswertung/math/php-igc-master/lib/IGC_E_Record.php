<?php
/**
 * IGC_E_Record class extends IGC_Record. The E Record (Event) must immediately
 * precede a B Record which logs where the event occurred.
 *
 * @see IGC_Record
 *
 * @version 0.1
 * @author Mike Milano <coder1@gmail.com>
 * @project php-igc
 */
class IGC_E_Record extends IGC_Record
{
  /**
   * Time array [0] hours, [1] minutes, [3] seconds
   *
   * @access public
   * @var array
   */
  public $time_array;

  /**
   * Mnemonic
   *
   * @access public
   * @var string
   */
  public $mnemonics;

  /**
   * Comment
   *
   * @access public
   * @var string
   */
  public $comment;

  /**
   * Class constructor creates the E record from the raw IGC string
   *
   * @param        string  $record
   */
  public function __construct($record)
  {
    $this->type = 'E';
    $this->raw = $string;

    $this->time_array['h'] = substr($record,1,2);
    $this->time_array['m'] = substr($record,3,2);
    $this->time_array['s'] = substr($record,5,2);

    $this->mnemonics = substr($record,7,3);
    $this->comment = substr($record,10);
  }
}
?>
