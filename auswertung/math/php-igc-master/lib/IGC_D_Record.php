<?php
/**
 * IGC_D_Record class extends IGC_Record. Differential GPS
 *
 * @see IGC_Record
 *
 * @version 0.1
 * @author Mike Milano <coder1@gmail.com>
 * @project php-igc
 */
class IGC_D_Record extends IGC_Record
{
  /**
   * GPS Qualifier 1: GPS, 2:DGPS
   *
   * @access public
   * @var integer
   */
  public $gps_qualifier;

  /**
   * Station ID
   *
   * @access public
   * @var string
   */
  public $station_id;

  /**
   * Class constructor creates the D record from the raw IGC string
   *
   * @param     string  $record
   */
  public function __construct($record)
  {
    $this->type = 'D';
    $this->raw = $string;

    $this->gps_qualifier = substr($record,1,1);
    $this->station_id = substr($record,2,4);
  }
}
?>
