<?php
/**
 * IGC_H_Record class extends IGC_Record. The H records contain header information.
 *
 * @see IGC_Record
 *
 * @version 0.1
 * @author Mike Milano <coder1@gmail.com>
 * @project php-igc
 */
class IGC_H_Record extends IGC_Record
{
  /**
   * Source
   *
   * @access public
   * @var string
   */
  public $source;

  /**
   * Three letter code
   *
   * @access public
   * @var string
   */
  public $tlc;

  /**
   * Key
   *
   * @access public
   * @var string
   */
  public $key;

  /**
   * Value
   *
   * @access public
   * @var string
   */
  public $value;

  /**
   * Class constructor creates the H record from the raw IGC string
   *
   * @param     string  $record
   */
  public function __construct($record)
  {
    $this->type = 'H';
    $this->raw = $record;

    $this->source = substr($record, 1, 1);
    $this->tlc = substr($record, 2, 3);

    if ($this->tlc == "DTE" || $this->tlc == "FXA") {
      $this->key = substr($record, 0, 5);
      $this->value = substr($record, 5);
    }
    else {
      // explode the rest of the string on the colon
      $array = explode(":", substr($record, 4));
      $this->key = $array[0];
      $this->value = $array[1];
    }
  }
}
?>
