<?php
/**
 * IGC_A_Record class extends IGC_Record. The A Record is the first record in an IGC Data File.
 *
 * @see IGC_Record
 *
 * @version 0.1
 * @author Mike Milano <coder1@gmail.com>
 * @project php-igc
 */
class IGC_A_Record extends IGC_Record
{
  /**
   * The single byte manufacturer code. Use PHP_IGC::GetManufacturerFromCode() for full string.
   * @access public
   * @var string
   */
  public $manufacturer;

  /**
   * The 5 byte equipment ID
   * @access public
   * @var string
   */
  public $unique_id;

  /**
   * ID Extension. (???)
   * @access public
   * @var string
   */
  public $id_extension;

  /**
   * Class constructor creates the A record from the raw IGC string
   *
   * @param        string  $record
   */
  public function __construct($record)
  {
    $this->type = 'A';
    $this->raw = $record;

    $this->manufacturer = substr($record,1,1);
    $this->unique_id = substr($record,2,5);
    $this->id_extension = substr($record,6);
  }
}
?>
