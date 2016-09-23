<?php
/**
 * IGC_B_Record class extends IGC_Record. The mandatory data is: UTC, latitude, longitude, fix validity
 * and pressure altitude. It is recommended to include GPS altitude and fix accuracy if they are available.
 * The B Record is a multiple instance record.
 *
 * @see IGC_Record
 *
 * @version 0.1
 * @author Mike Milano <coder1@gmail.com>
 * @project php-igc
 */
class IGC_B_Record extends IGC_Record
{
  /**
   * Time array [0] hours, [1] minutes, [3] seconds
   *
   * @access public
   * @var array
   */
  public $time_array;

  /**
   * Latitude array [0] degrees, [1] minutes, [2] decimal minutes, [3] direction
   *
   * @access public
   * @var array
   */
  public $latitude;

  /**
   * Longitude array [0] degrees, [1] minutes, [2] decimal minutes, [3] direction
   *
   * @access public
   * @var array
   */
  public $longitude;

  /**
   * Fixed valid. A: valid, V:nav warning
   *
   * @access public
   * @var string
   */
  public $fixed_valid;

  /**
   * Pressure Altitude
   *
   * @access public
   * @var integer
   */
  public $pressure_altitude;

  /**
   * GPS Altitude
   *
   * @access public
   * @var integer
   */
  public $gps_altitude;

  /**
   * Fix Accuracy
   *
   * @access public
   * @var integer
   */
  public $fix_accuracy;

  /**
   * Extensions in the B records and their positions in the record
   * string
   *
   * @access private
   * @var array
   */
  private static $extension_offsets = array();

  /**
   * Class constructor creates the B record from the raw IGC string
   *
   * @param     string  $record
   */
  public function __construct($record)
  {
    $this->type = 'B';
    $this->raw = $record;

    $this->time_array['h'] = substr($record,1,2);
    $this->time_array['m'] = substr($record,3,2);
    $this->time_array['s'] = substr($record,5,2);

    // set degrees, minutes, and decimal minutes
    // latitude
    $this->latitude = array();
    $this->latitude['degrees'] = substr($record,7,2);
    $this->latitude['minutes'] = substr($record,9,2);
    $this->latitude['decimal_minutes'] = substr($record,11,3);
    $this->latitude['direction'] = substr($record,14,1);

    $pm = $this->latitude['direction']=="S"?"-":"";
    $dd = (($this->latitude['minutes'].".".$this->latitude['decimal_minutes'])/60)+$this->latitude['degrees'];
    $this->latitude['decimal_degrees'] = $pm.$dd;

    // longitude
    $this->longitude = array();
    $this->longitude['degrees'] = substr($record,15,3);
    $this->longitude['minutes'] = substr($record,18,2);
    $this->longitude['decimal_minutes'] = substr($record,20,3);
    $this->longitude['direction'] = substr($record,23,1);

    $pm = $this->longitude['direction']=="W"?"-":"";
    $dd = (($this->longitude['minutes'].".".$this->longitude['decimal_minutes'])/60)+$this->longitude['degrees'];
    $this->longitude['decimal_degrees'] = $pm.$dd;

    // set fixed valid
    $this->fixed_valid = substr($record, 24, 1);

    // set pressure altitude
    $this->pressure_altitude = intval(substr($record, 25, 5));

    // set gps altitude
    $this->gps_altitude = intval(substr($record, 30, 5));

    foreach (self::$extension_offsets as
             $extension_name => $extension_offset) {
      $this->extension_data[$extension_name] =
        substr($record, $extension_offset["start_byte"],
               $extension_offset["finish_byte"] -
               $extension_offset["start_byte"]);
    }
  }

  /**
   * Set expected extension fields in the B records
   *
   * @param     array  $extensions
   */
  public static function SetExtensionOffsets($extension_offsets)
  {
    self::$extension_offsets = $extension_offsets;
  }
}
?>
