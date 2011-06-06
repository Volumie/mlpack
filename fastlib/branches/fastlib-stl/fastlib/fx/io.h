/***
 * @file io.h
 * @author Matthew Amidon 
 * 
 * This file implements the IO subsystem which is intended to replace FX.
 * This can be used more or less regardless of context.  In the future, 
 * it might be expanded to include file I/O.
 */

#ifndef MLPACK_IO_IO_H
#define MLPACK_IO_IO_H

#include <boost/program_options.hpp>
#include <boost/any.hpp>
#include <iostream>
#include <map>
#include <string>
#include <list>

#include "optionshierarchy.h"
#include "prefixedoutstream.h"
#include "nulloutstream.h"
#include "io_deleter.h" // To make sure we can delete the singleton.

/***
 * This macro is used as shorthand for documenting the program.  Only one of
 * these should be present in your program!  Therefore, use it in the main.cc
 * (or corresponding executable) in your program.
 *
 * @param NAME Short string representing the name of the program.
 * @param DESC Long string describing what the program does and possibly a
 *     simple usage example.  Newlines should not be used here; this is taken
 *     care of by IO.
 */
#define PROGRAM_INFO(NAME, DESC) static mlpack::ProgramDoc \
    io_programdoc_dummy_object = mlpack::ProgramDoc(NAME, DESC);

/***
 * These defines facilitate the registering of command line options.  Use the
 * macro which specifies the type of the option you want to add.  Default values
 * are not used for required parameters (since they are required).
 *
 * @param ID Name of the parameter.
 * @param DESC Quick description of the parameter.
 * @param PARENT Parent module of the parameter.
 * @param DEF Default value of the parameter (used if the parameter is not
 *   specified on the command line).
 *
 * The parameter will then be specified with --PARENT/ID=value.
 */
#define PARAM_FLAG(ID, DESC, PARENT) PARAM(bool, ID, DESC, PARENT, false, false)
#define PARAM_INT(ID, DESC, PARENT, DEF) \
    PARAM(int, ID, DESC, PARENT, DEF, false)
#define PARAM_FLOAT(ID, DESC, PARENT, DEF) \
    PARAM(float, ID, DESC, PARENT, DEF, false)
#define PARAM_STRING(ID, DESC, PARENT, DEF) \
    PARAM(std::string, ID, DESC, PARENT, DEF, false)
#define PARAM_VECTOR(T, ID, DESC, PARENT) \
    PARAM(std::vector<T>, ID, DESC, PARENT, std::vector<T>(), false)

// Required bool?  Probably not necessary.
//#define PARAM_BOOL_REQ(ID, DESC, PARENT) PARAM(bool, ID, DESC, PARENT, false,
//  true)
#define PARAM_INT_REQ(ID, DESC, PARENT) PARAM(int, ID, DESC, PARENT, 0, true)
#define PARAM_FLOAT_REQ(ID, DESC, PARENT) PARAM(float, ID, DESC, PARENT, 0.0f, \
  true)
#define PARAM_STRING_REQ(ID, DESC, PARENT) PARAM(std::string, ID, DESC, \
    PARENT, "", true);
#define PARAM_VECTOR_REQ(T, ID, DESC, PARENT) PARAM(std::vector<T>, ID, DESC, \
    PARENT, std::vector<T>(), true);

/// These are ugly, but necessary utility functions we must use to generate a
/// unique identifier inside of the PARAM() module.
#define JOIN(x, y) JOIN_AGAIN(x, y)
#define JOIN_AGAIN(x, y) x ## y

/***
 * Define an input parameter.  Don't use this function, use the other ones above
 * that call it.  Note that we are using the CURRENT_PARAM_NUM macro for naming
 * these actual parameters, which is a bit of an ugly hack... but this is the
 * preprocessor, after all.  We don't have much choice other than ugliness.
 * 
 * @param T type of parameter
 * @param ID name of parameter (for --help, pass "help")
 * @param DESC description of parameter (string)
 * @param PARENT name of parent module (string)
 * @param DEF default value of the parameter
 * @param REQ whether or not parameter is required (bool)
 */
#ifdef __COUNTER__
  #define PARAM(T, ID, DESC, PARENT, DEF, REQ) static mlpack::Option<T> \
      JOIN(io_option_dummy_object_, __COUNTER__) \
      (false, DEF, ID, DESC, PARENT, REQ);

  #define PARAM_MODULE(ID, DESC) static mlpack::Option<int> \
      JOIN(io_option_module_dummy_object_, __COUNTER__) (true, 0, ID, DESC, \
      NULL);
#else
  // We have to do some really bizarre stuff since __COUNTER__ isn't defined.  I
  // don't think we can absolutely guarantee success, but it should be "good
  // enough".  We use the last bits the __LINE__ macro and the type of the
  // parameter to try and get a good guess at something unique.
  #define PARAM(T, ID, DESC, PARENT, DEF, REQ) static mlpack::Option<T> \
      JOIN(JOIN(io_option_dummy_object_, __LINE__), opt) (false, DEF, ID, \
      DESC, PARENT, REQ);

  #define PARAM_MODULE(ID, DESC) static mlpack::Option<int> \
      JOIN(JOIN(io_option_dummy_object_, __LINE__), mod) (true, 0, ID, DESC, \
      NULL);
#endif

/***
 * The TYPENAME macro is used internally to convert a type into a string.
 */
#define TYPENAME(x) (std::string(typeid(x).name()))

namespace po = boost::program_options;

namespace mlpack {

// Externally defined in option.h, this class holds information about the
// program being run.
class ProgramDoc;

class IO {
 public:
  /*
   * Adds a parameter to the heirarchy. Use char* and not 
   * std::string since the vast majority of use cases will 
   * be literal strings.
   * 
   * 
   * @param identifier The name of the parameter.
   * @param description Short string description of the parameter.
   * @param parent Full pathname of a parent module, default is root node.
   * @param required Indicates if parameter must be set on command line.
   */
  static void Add(const char* identifier, 
                  const char* description, 
                  const char* parent=NULL, 
                  bool required = false);
    
  /*
   * Adds a parameter to the heirarchy. Use char* and not 
   * std::string since the vast majority of use cases will 
   * be literal strings.
   * If the argument requires a parameter, you must specify a type
   * 
   * @param identifier The name of the parameter.
   * @param description Short string description of the parameter.
   * @param parent Full pathname of a parent module, default is root node.
   * @param required Indicates if parameter must be set on command line.
   */
  template<class T>
  static void Add(const char* identifier, 
                  const char* description, 
                  const char* parent, 
                  bool required=false); 
                   
  /* 
   * See if the specified flag was found while parsing. 
   * 
   * @param identifier The name of the parameter in question. 
   */
  static bool CheckValue(const char* identifier);
      
      
  /* 
   * Parses the parameters for 'help' and 'info' 
   * If found, will print out the appropriate information
   * and kill the program.
   */
  static void DefaultMessages();
   
  /*
   * Grab the value of type T found while parsing.  
   * Should use checkValue first.
   *
   * @param argc The number of arguments on the commandline.
   * @param argv The array of arguments as strings 
   */
  template<typename T>
  static T& GetValue(const char* identifier); 
   
  /*
   * Grab the description of the specified node.
   * 
   * @param identifier Name of the node in question.
   * @return Description of the node in question. 
   */
  static std::string GetDescription(const char* identifier);

  /*
   * Parses the commandline for arguments.
   *
   * @param argc The number of arguments on the commandline.
   * @param argv The array of arguments as strings 
   */
  static void ParseCommandLine(int argc, char** argv);
    
  /*
   * Parses a stream for arguments
   *
   * @param stream The stream to be parsed.
   */
  static void ParseStream(std::istream& stream);
      
  /* Prints out the current hierachy */
  static void Print();

// We only use PrefixedOutStream if the program is compiled with debug symbols.
#ifdef DEBUG
  // Prints debug output with the appropriate tag.
  static io::PrefixedOutStream Debug;
#else
  // Dumps debug output into the bit nether regions.
  static io::NullOutStream Debug;
#endif
  // Prints output with their respective tags of [INFO], [WARN], and [FATAL]
  static io::PrefixedOutStream Info;
  static io::PrefixedOutStream Warn;
  static io::PrefixedOutStream Fatal;
  static std::ostream& cout; 
   
  /*
   * Checks that all parameters specified as required 
   * have been specified on the command line.
   * If they havent, prints an error message and kills the
   * program.
   */
  static void RequiredOptions();

  /* Cleans up input pathnames, rendering strings such as /foo/bar
     and foo/bar/ equivalent inputs */
  static std::string SanitizeString(const char* str);
 
  /*
   * Initializes a timer, available like a normal value specified on 
   * the command line.  Timers are of type timval
   *
   * @param timerName The name of the timer in question.
   */
  static void StartTimer(const char* timerName);
     
  /* 
   * Halts the timer, and replaces it's value with 
   * the delta time from it's start 
   *
   * @param timerName The name of the timer in question.
   */
  static void StopTimer(const char* timerName);
   
  /*
   * Parses the values given on the command line,
   * overriding any default values.
   */
  static void UpdateGmap();
  
  /**
   * Registers a ProgramDoc object, which contains documentation about the
   * program.  If this method has been called before (that is, if two
   * ProgramDocs are instantiated in the program), a fatal error will be thrown.
   *
   * @param doc Pointer to the ProgramDoc object.
   */
  static void RegisterProgramDoc(ProgramDoc* doc);

  /***
   * Destroy the IO object.  This resets the pointer to the singleton, so in
   * case someone tries to access it after destruction, a new one will be made
   * (the program will not fail).
   */
  static void Destroy();

  // Destructor
  ~IO();
  
 private:
  // The documentation and names of options
  po::options_description desc;
    
  // Values of the options given by user
  po::variables_map vmap;

  // Pathnames of required options
  std::list<std::string> requiredOptions;
    
  // Map of global values, stored here instead of in OptionsHierarchy
  // For ease of implementation
  std::map<std::string, boost::any> globalValues;
      
  // Store a relative index of path names
  io::OptionsHierarchy hierarchy;
      
  // The singleton, obviously
  static IO* singleton;

 public:
  // Pointer to the ProgramDoc object.
  ProgramDoc *doc;
  
 private:  
  /* 
   * Not exposed to the outside, so as to spare users some ungainly
   * x.GetSingleton().foo() syntax.
   * In this case, the singleton is used to store data for the static methods, 
   * as there is no point in defining static methods only to have users call 
   * private instance methods 
   *
   * Returns the singleton instance for use in the static methods 
   */
  static IO& GetSingleton();
   
  /* 
   * Properly formats strings such that there aren't too few or too many '/'s.
   * 
   * @param id The name of the parameter, eg bar in foo/bar.
   * @param parent The full name of the parameter's parent, 
   *   eg foo/bar in foo/bar/buzz.
   * @param tname String identifier of the parameter's type.
   * @param description String description of the parameter.
   */
  std::string ManageHierarchy(const char* id, 
                              const char* parent, 
                              std::string& tname, 
                              const char* description = "");
    
  /* Make the constructor private, to preclude unauthorized instances */
  IO();
   
  /* 
   * Initialize desc with a particular name.
   * 
   * @param optionsName Name of the module, as far as boost is concerned.
   */ 
  IO(std::string& optionsName);

  // Private copy constructor; don't want copies floating around.
  IO(const IO& other);
};

}; // namespace mlpack

// Include the actual definitions of templated methods 
#include "io_impl.h"

#endif
