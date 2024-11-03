//
//   ChangeInputDacOffset.cc
//
// Jan. 16, 2016, Y.Nakada
//


// This program add common offset to each input DAC value.
//
// instruction of options
//       -a : DAC values of the all channels are changed corresponding to the offset value.
//       -u : DAC values of the upper channels (0--31) are changed corresponding to the offset value.
//       -d : DAC values of the lower channels (32--63) are changed corresponding to the offset value.
//       -i<ch1> -f<ch2> :  DAC values of the range <ch1> to <ch2> are changed corresponding to the offset value.
//       --  : This expression MUST be writen in the end of the option array. If this option absent, a negative offset is regarded as an option.


#include <unistd.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <vector>

int main( int argc, char **argv )
{
  std::string filepath;
  int offset = 0;
  int range_ch[2] = { -1, -1 };

  int opt;
  bool flag_valid = true;
  bool flag_aud   = false;
  bool flag_if    = false;
  opterr = 0; // an extern variable in unistd.h
  while ( ( opt = getopt( argc, argv, "audi:f:" ) ) != -1 ) {
    switch ( opt ) {
    case 'a': 
      if ( !flag_aud && !flag_if ) {
	range_ch[0] = 0;  range_ch[1] = 63;
      } else { flag_valid = false; }
      flag_aud = true;
      break;
    case 'u': 
      if ( !flag_aud && !flag_if ) {
	range_ch[0] = 0;  range_ch[1] = 31; 
      } else { flag_valid = false; }
      flag_aud = true;
      break;
    case 'd': 
      if ( !flag_aud && !flag_if ) {
	range_ch[0] = 32; range_ch[1] = 63;
      } else { flag_valid = false; }
      flag_aud = true;
      break;
    case 'i': 
      if ( !flag_aud ) {
	range_ch[0] = std::atoi( optarg ); // an extern variable in unistd.h
      } else { flag_valid = false; }
      flag_if = true;
      break;
    case 'f': 
      if ( !flag_aud ) {
	range_ch[1] = std::atoi( optarg ); // an extern variable in unistd.h
      } else { flag_valid = false; }
      flag_if = true;
      break;
    default:
      std::cerr 
	<< "[ChangeInputDacOffset] USAGE: [prompt]$ ./ChangeInputDacOffset <option> -- <file path> <offset value>" << '\n'
	<< "[ChangeInputDacOffset] Note that an option MUST be explicitely chosen." 
	<< std::endl;
      abort();
    }
  }
  //  if ( range_ch[0] > range_ch[1] ) {
  if ( !flag_valid ||
       range_ch[0] == -1 || range_ch[1] == -1 || range_ch[0] > range_ch[1]
      ) {
    std::cerr 
	<< "[ChangeInputDacOffset] ERROR: Invalid option usage."
	<< std::endl;
    abort();
  }
//   std::cout << "optind: " << optind << '\t' << "argc: " << argc << std::endl;
//    for ( int i = 0; i < argc; ++i ) {
//      std::cout << "i: " << i << '\t' << argv[i] << std::endl;
//    }
//  if ( optind == 3 && argc == 5 ) {
  if ( argc - optind == 2 ) {
    filepath = argv[optind];
    offset   = std::atoi( argv[optind + 1] );
    std::cout 
      << "[ChangeInputDacOffset] file name: " << filepath << "    "
      << "offset value: " << offset 
      << std::endl;
  } else {
    std::cerr 
      << "[ChangeInputDacOffset] USAGE: [prompt]$ ./ChangeInputDacOffset <option> -- <file path> <offset value>" << '\n'
      << "[ChangeInputDacOffset] Note that an option MUST be explicitely chosen." 
      << std::endl;
      abort();
  }

  if ( abs(offset) > 255 ) {
    std::cerr 
      << "[ChangeInputDacOffset] ERROR: Too high/low offset of " << offset << "."
      << std::endl;
    abort();
  }

  //// file reading ///////////////////////////////////////////////////////////
  std::ifstream infile( filepath.c_str() );
  std::vector< std::string > line;
  std::vector< bool > flag_line;
  std::vector< int > dac;
  if ( !infile.is_open() ) {
    std::cerr 
      << "[ChangeInputDacOffset] ERROR: Could not access the file \"" << filepath << "\"."
      << std::endl;
    abort();
  } else {
    std::string buff_line;
    while ( getline( infile, buff_line ) ) {
      std::stringstream ss( buff_line );
      std::string buff;
      std::vector< std::string > elem_line;
      while ( ss >> buff ) {
	elem_line.push_back( buff );
      }
      if ( elem_line.at( 0 ) == "-" ) {
	dac.push_back( std::atoi( elem_line.at( 1 ).c_str() ) );
	flag_line.push_back( true );
      } else {
	flag_line.push_back( false );
      }
      line.push_back( buff_line );
    } // if ( getline(...) )
  } // if ( !infile.is_open() )

  //// file writing ///////////////////////////////////////////////////////////
  if ( dac.size() != 64 ) {
    std::cerr 
      << "[ChangeInputDacOffset] ERROR: Invalid number of input DAC values in the file."
      << std::endl;
    abort();
  } else {
    std::string outfile_name( filepath + ".offset_added" );
    std::ofstream outfile( outfile_name.c_str() );
    //    std::vector< int >::iterator itr_dac( dac.begin() );
    int index_dac = 0;
    std::cout << "[ChangeInputDacOffset] ================\n";
    std::cout << "[ChangeInputDacOffset]  Target Channel \n";
    std::cout << "[ChangeInputDacOffset] ----------------\n";
    for ( int i = 0; i < flag_line.size(); ++i ) {
      if ( flag_line.at( i ) ) {
	int dac_new = dac.at( index_dac ) + offset;
	//	int dac_new = *itr_dac + offset;
	if ( dac_new < 260 || dac_new > 500 ) {
	  outfile << " - " << dac.at( index_dac ) << std::endl;
	  //	  outfile << " - " << *itr_dac << std::endl;
	  std::cerr 
	    << "\n"
	    << "[ChangeInputDacOffset] ERROR: The new input DAC value of " 
	    << dac.at( index_dac ) << " + " << offset
	    //	    << *itr_dac << " + " << offset
	    << " at line " << i + 1 << ' '
	    << "exceeded the allowed range of 260 to 500." << '\n'
	    << "[ChangeInputDacOffset] The new input DAC value remained the original one."
	    << std::endl;
	} else {
	  if ( index_dac >= range_ch[0] && index_dac <= range_ch[1] ) {
	    outfile << "  - " << dac.at( index_dac ) + offset << std::endl;
	    std::cout << "[ChangeInputDacOffset]        " << index_dac
		      << std::endl;

	    //	  outfile << " - " << *itr_dac + offset << std::endl;
	  } else {
	    outfile << line.at( i ) << std::endl;
	  }
	}
	++index_dac;
	//	++itr_dac;
      } else {
 	outfile << line.at( i ) << std::endl;
      }
    }
    std::cout << "[ChangeInputDacOffset] ================" << std::endl;;
    std::cout
      << "[ChangeInputDacOffset] The new input DAC file \"" << outfile_name << "\" has been created."
      << std::endl;
  }
  

}
