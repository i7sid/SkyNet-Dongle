<?php
/**
 * IGC_K_Record class extends IGC_Record. Extension Data. The information in the K Record is specified by the J Record.
 *
 * @see IGC_Record
 *
 * @version 0.1
 * @author Mike Milano <coder1@gmail.com>
 * @project php-igc
 */
class IGC_K_Record extends IGC_Record
{
  /**
   * Time array [0] hours, [1] minutes, [3] seconds
   *
   * @access public
   * @var array
   */
  public $time_array;

  /**
   * Total Energy Altitude
   *
   * @access public
   * @var integer
   */
  public $total_energy_altitude;

  /**
   * Class constructor creates the K record from the raw IGC string
   *
   * @param     string  $record
   */
  public function __construct($record)
  {
    $this->type = 'F';
    $this->raw = $record;

    $this->time_array['h'] = substr($record,1,2);
    $this->time_array['m'] = substr($record,3,2);
    $this->time_array['s'] = substr($record,5,2);

    $this->total_energy_altitude = substr($record,7,4);
  }
}
?>
