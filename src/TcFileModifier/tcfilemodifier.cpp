#include"tcfilemodifier.hpp"
//#include"External/FileReader/FilesSpecificData/CANoeFilesFormat/TestEnvironment/FRI_FSD_CANoe_TestEnvironment.hpp"
//#include"External/FileReader/FilesSpecificData/XML/VSysvar/FRI_FSD_XML_VSysVar.hpp"
#include"External/RegExpCore/regexprcore.hpp"
#include"TclInterpreter/tclToCAPL.hpp"
#include"Tcl2Capl/userinputconfig.hpp"

template <>
QString FSD_ByLine_TcFileModifierData::Config::fileFilter(){
    return QString("*.tc;*.tcl");
}
// TODO: Upewnij sie ze dla tcl nie beda generowane testy

using Format = Format;
using ActionStat = ActionStat;
using TC_Info_Data = TC_Info_Data;
using RuleControl = ModifierRuleControl;
using CAN = CAN;

const QStringList TcFileModifierConfigBase::Format::targetMap{
    "Raw",
    "SplittedRaw"
};

CAN::VtSignals CAN::_signals =
{
    {   // ATCVT_RangeIndicationExtended
        "ATCVT_RangeIndicationExtended",
        { "P_range", "R_range", "N_range",
            "D_range", "S_range", "L_range",
            "B_range", "1st_range", "2nd_range",
            "3rd_range", "4th_range", "5th_range",
            "6th_range", "7th_range", "8th_range",
            "9th_range", "10th_range", "M1st",
            "M2nd", "M3rd", "M4th" ,
            "M5th", "M6th", "M7th",
            "M8th", "M9th", "M10th",
            "Auto_1st", "Auto_2nd", "Auto_3rd",
            "Auto_4th", "Auto_5th", "Auto_6th",
            "Auto_7th", "Auto_8th", "Auto_9th",
            "Auto_10th"}
    },
    {   // BrakeInfoStatus
        "BrakeInfoStatus",
        {
            "Brake_pedal_not_pressed",
            "Brake_pedal_pressed",
            "Brake_pedal_confirmed_pressed",
            "unavailable"
        }
    },
    {   // ChargingSoundRequest
        "ChargingSoundRequest",
        {"Charging_No_request",
            "Play_charging_plug_sound_request",
            "Play_Admit_to_charge_battery_",
            "Play_Unlock_charging_plug_sound_"}
    },
    {   // DayNightForBacklights
        "DayNightForBacklights",
        {
            "Day",
            "Twilight",
            "Night",
            "Unavailable"
        }
    },
    {   // GADE
        "GADE",
        {
            "Sleep_or_Crank_fault_Disable",
            "AutoACC_fault_Disable",
            "ALL_fault_Disable",
            "ALL_fault_Enable",
            "Unavailable"
        }
    },
    {   // PeriodicReverseBuzzerTrigger
        "PeriodicReverseBuzzerTrigger",
        {"No_request",
            "Reverse_alert_trigger"}
    },
    {   // PowerTrainStatus
        "PowerTrainStatus",
        {"Powertrain_Stopped",
            "Reserved",
            "Powertrain_Running",
            "Powertrain_Starting"}
    },
    {
        "StopStartInformationDisplay",
        { "xEV_Driver_Exit_Alert_level_1",
            "xEV_Driver_Exit_Alert_level_2"}
    },
    {
        "VSPstatusDisplay",
        { "VSP_deactivated_not_request",
            "Description for the value '0x1'"}
    }
};

// PHASES ---------------------------------------------
//const FSD_ByLine_TcFileModifierData::Data::ModifierPhases FSD_ByLine_TcFileModifierData::Data::phases =
//{
const ModifierPhase _PHASE_TEST_CASE_INFO =
{
    { // Phase 1: BugFinder Testcase Description
        { // #@NAME (arg1 = String)
          { // Conditions (1)
            {
                ActionStat::SPLIT, {"\\s+"}
            },
            {   // (1) - Starts With "#@NAME"
                ActionStat::STARTS_WITH, {"#@NAME"}
            },
          },
          { // Actions (2)
            {
                ActionStat::WRITE_TO_TC_INFO, {QString::number(static_cast<uint>(TC_Info_Data::NAME)),
                                               "", "=1"}
            }
          }
        },
        {   // #@TITLE (arg1 = String)
            {   // Conditions (1)
                {   // (1) Starts With "#@TITLE"
                    ActionStat::STARTS_WITH, {"#@TITLE"}
                },
            },
            {   // Actions (2)
                {
                    ActionStat::WRITE_TO_TC_INFO, {QString::number(static_cast<uint>(TC_Info_Data::TITLE)),
                                                   "", "@ ",
                                                   "", ">0"}
                }
            }
        },
        {   // #@DESC (arg1 = String)
            {   // Conditions (1)
                {   // (1) Starts With "#@DESC"
                    ActionStat::STARTS_WITH, {"#@DESC"}
                },
            },
            {   // Actions (2)
                {
                    ActionStat::WRITE_TO_TC_INFO, {QString::number(static_cast<uint>(TC_Info_Data::DESCRIPTION)),
                                                   "", "@ ",
                                                   "", ">0"}
                }
            }
        },
        {   // Ignore single #@DOMAIN
            {   // Conditions (1)
                {   // (1) Compare all With "#@DOMAIN"
                    ActionStat::COMPARE,
                    {
                        QString::number(1), // Compare Strings list size
                        "#@DOMAIN",
                        QString::number(4), // Formated string size
                        "", QString("T") + Format::cast_target_str(Format::Target::RAW),
                        "", QChar(static_cast<std::underlying_type_t<Format::Rule>>(Format::Rule::INDEX_OR_FULL_LINE))
                    }
                },
            },
            {   // Actions (0)
                // No Actions
            }
        },
        {   // #@DOMAIN (arg1 = String)
            {   // Conditions (1)
                {   // (1) Starts With "#@DOMAIN"
                    ActionStat::STARTS_WITH, {"#@DOMAIN"}
                },
            },
            {   // Actions (2)
                {
                    ActionStat::WRITE_TO_TC_INFO, {QString::number(static_cast<uint>(TC_Info_Data::TC_DOMAIN)),
                                                   "", "=1"}
                }
            }
        },
        {   // #@INTG (arg1 = String)
            {   // Conditions (1)
                {   // (1) Starts With "#@INTG"
                    ActionStat::STARTS_WITH, {"#@INTG"}
                },
            },
            {   // Actions (2)
                {
                    ActionStat::WRITE_TO_TC_INFO, {QString::number(static_cast<uint>(TC_Info_Data::INTEGRATION)),
                                                   "", "=1"}
                }
            }
        },
        {   // #@REGS (arg1 = String)
            {   // Conditions (1)
                {   // (1) Starts With "#@REGS"
                    ActionStat::STARTS_WITH, {"#@REGS"}
                },
            },
            {   // Actions (2)
                {
                    ActionStat::WRITE_TO_TC_INFO, {QString::number(static_cast<uint>(TC_Info_Data::REGRESSION)),
                                                   "", "=1"}
                }
            }
        },
        {   // #@REQ (arg1 = String)
            {   // Conditions (1)
                {   // (1) Starts With "#@REQ"
                    ActionStat::STARTS_WITH, {"#@REQ"}
                },
            },
            {   // Actions (2)
                {
                    ActionStat::WRITE_TO_TC_INFO, {QString::number(static_cast<uint>(TC_Info_Data::REQUIREMENTS)),
                                                   "", "@; ",
                                                   "", ">0"}
                }
            }
        },
        {   // #@DOC (arg1 = String)
            {   // Conditions (1)
                {   // (1) Starts With "#@DOC"
                    ActionStat::STARTS_WITH, {"#@DOC"}
                },
            },
            {   // Actions (2)
                {
                    ActionStat::WRITE_TO_TC_INFO, {QString::number(static_cast<uint>(TC_Info_Data::DOCUMENTS)),
                                                   "", "@; ",
                                                   "", ">0"}
                }
            }
        },
          {   // #@AUTHOR (arg1 = String)
              {   // Conditions (1)
                  {   // (1) Starts With "#@AUTHOR"
                      ActionStat::COMPARE, {"1","#@AUTHOR",
                                            "4","", "T" + Format::cast_target_str(Format::Target::RAW),
                                            "", "="}
                  },
              },
              {   // Actions (2)

              }
          },
        {   // #@AUTHOR (arg1 = String)
            {   // Conditions (1)
                {   // (1) Starts With "#@AUTHOR"
                    ActionStat::STARTS_WITH, {"#@AUTHOR"}
                },
            },
            {   // Actions (2)
                {
                    ActionStat::WRITE_TO_TC_INFO, {QString::number(static_cast<uint>(TC_Info_Data::AUTHOR)),
                                                   "", "=1"}
                }
            }
        },
        {   // #@TYPE (arg1 = String)
            {   // Conditions (1)
                {   // (1) Starts With "#@TYPE"
                    ActionStat::STARTS_WITH, {"#@TYPE"}
                },
            },
            {   // Actions (2)                
                {
                    ActionStat::WRITE_TO_TC_INFO, {QString::number(static_cast<uint>(TC_Info_Data::TYPE)),
                                                   "", "=1"}
                }
            }
        },
          {   // #@VER %version: (arg2 = String) %
              {   // Conditions (1)
                  {   // (1) Starts With "#@VER"
                      ActionStat::STARTS_WITH, {"#@VER"}
                  },
                  {
                      ActionStat::COMPARE, {"1", "%version:",
                                            "2", "", "=1"}
                  }
              },
              {   // Actions (2)
                  {
                      ActionStat::WRITE_TO_TC_INFO, {QString::number(static_cast<uint>(TC_Info_Data::VERSION)),
                                                     "", "=2"}
                  }
              }
          },
        {   // #@VER %version: (arg2 = String) %
            {   // Conditions (1)
                {   // (1) Starts With "#@VER"
                    ActionStat::STARTS_WITH, {"#@VER"}
                },
            },
            {   // Actions (2)                
                {
                    ActionStat::WRITE_TO_TC_INFO, {QString::number(static_cast<uint>(TC_Info_Data::VERSION)),
                                                   "", "=1"}
                }
            }
        },
      { // Any #@ -> Ignore it
        {   // Condition (1)
            {   // (1) Starts With "#@"
                ActionStat::STARTS_WITH, {"#@"}
            },
        },
        {   // Actions (1)
            {   // (1): Comment Out
                ActionStat::COMMENT_OUT, {}
            }
        }
      }
    },
    {   //On End
        {   // On No Rules Actions
            {   // Actions (2)
                {   // (2): Phase CHANGE_PHASE
                    ActionStat::CHANGE_PHASE, { "Main", QString::number(true)}
                }
            }
        },
        {   // Actions after rule check

        }
    }
}
;
//,
const ModifierPhase _PHASE_STANDARD =
{
    {   // Phase 2 ___
        /*{   // Rule 1 (Comment # -> Comment //)
            {   // Conditions (1)
                { // (1): Starts With #
                    ActionStat::STARTS_WITH, {"#"}
                }
            },
            {   // Actions (1) : Comment Out
                {
                    ActionStat::COMMENT_OUT, {}
                }
            }
        },*/
        /*{   // Replace Empty Line to New Line
            {
                {
                    ActionStat::COMPARE, {"1",
                                          "",
                                          "4", "","T" + Format::cast_target_str(Format::Target::RAW),
                                          "", "="}
                }
            },
            {
                {ActionStat::WRITE, {"\n"}},
            },
        },*/
/*
        {   // Replace "ALL_fault_|_Enable_DiagMuxDTC" with "ALL_fault_Enable"
            {
                {ActionStat::DEBUG, {}}   // As True
            },
            {
                {ActionStat::REPLACE_ALL, {"ALL_fault_Disable", "ALL_fault_|_Disable_DiagMuxDTC"}},
                {ActionStat::REPLACE_ALL, {"ALL_fault_Enable", "ALL_fault_|_Enable_DiagMuxDTC"}},
                {ActionStat::REPLACE_ALL, {"Sleep_or_Crank_fault_Disable", "Sleep_or_Crank_fault_|_Disable_DiagMuxDTC"}},
                {ActionStat::REPLACE_ALL, {"AutoACC_fault_Disable", "AutoACC_fault_|_Disable_DiagMuxDTC"}}
            },
            RuleControl::NO_BREAK_RULE_CHECK
        },
/*
        {   // Catch [expr or expr
            {
                {
                    ActionStat::CONTAINS, {"[expr", "expr"}
                }
            },
            {
                {
                    ActionStat::WRITE, {"// _PH_EXPR_ ",
                                        "", "T" + Format::cast_target_str(Format::Target::RAW),
                                        "", "=", "\n"}
                }
            }
        },*/
        /*{   // Rule : if
              {   // Conditions (1)
                  {   // (1) : Starts with if
                      ActionStat::STARTS_WITH, {"if"}
                  }
              },
              {   // Actions (2):
                  /*{
                      ActionStat::SPLIT, {"\\s+"}
                  },
                  {
                      ActionStat::CHANGE_PHASE, {QString::number(static_cast<uint>(Phase::IF)), QString::number(true)}
                  }
                  {
                      ActionStat::FORMAT
                  }
              }
        },
        {   // Rule : Replace with "" strings like "$::"
            {}, // No Conditions
            {   // Actions
                {
                    ActionStat::REPLACE_ALL, {"",
                                              "$::", "$"}
                }
            },
            RuleControl::NO_BREAK_RULE_CHECK
        },
        {   // Rule : stc_section RESULT
            {   // Conditions (1)
                {   // (1) : Starts with stc_section
                    ActionStat::STARTS_WITH, {"stc_section"}
                },
                {
                    ActionStat::COMPARE, {"1",
                                          "RESULT",
                                          "8", "","T" + Format::cast_target_str(Format::Target::RAW),
                                          "", "S\\s+",
                                          "", "T" + Format::cast_target_str(Format::Target::SPLITTED_RAW),
                                          "", "=1"}
                }
            },
            {   // Actions :
                {   // (2): Write "EXPECTED_TS" "(" "" ">2@ " ");\n"
                    ActionStat::WRITE, {"\tEXPECTED_TS(",
                                        "","T" + Format::cast_target_str(Format::Target::SPLITTED_RAW),
                                        "", "@ ",
                                        "", ">1",
                                        ");\n"}
                }
            }
        },
        {   // Rule : stc_section <Any>
            {   // Conditions (1)
                {   // (1) : Starts with stc_section
                    ActionStat::STARTS_WITH, {"stc_section"}
                }
            },
            {   // Actions (2):
                {   // (2): Write "" "=1" "(" "" ">2@ " ");\n"
                    ActionStat::WRITE, {"\t",
                                        "","T" + Format::cast_target_str(Format::Target::RAW),
                                        "", "S\\s",
                                        "", "T" + Format::cast_target_str(Format::Target::SPLITTED_RAW),
                                        "", "=1",
                                        "_TS(",
                                        "", "@ ",
                                        "", ">1",
                                        ");\n"}
                }
            }
        },
        {    // Rule 4: Call
            { // Conditions (1)
              {   // (1) : Starts with "call"
                ActionStat::STARTS_WITH, {"call"}
              }
            },
            {   // Actions (2)
                {
                    ActionStat::SPLIT, {"\\s+"}
                },
                {
                    ActionStat::CHANGE_PHASE, {QString::number(static_cast<uint>(Phase::CALL)), QString::number(true)}
                }
            }
        },
        {
            {
                {
                    ActionStat::STARTS_WITH, {"PROC_Drive_To_Default ON"}
                }
            },
            {
                {
                    ActionStat::WRITE, {"PROC_Drive_To_Default(`ON);"}
                }
            }
        },

        {   // All Functions (with arguments)
            { // Conditions (1)
              {   // (1) : Starts with "delay"
                ActionStat::STARTS_WITH, {"delay",
                                          "PROC_ResetModule",
                                          "PROC_RestartDevice",
                                          "PROC_SET_MASTER_AMPL_TO_ONE",
                                          "PROC_SET_MASTER_AMPL_CHARGING_TO_HALF",
                                          "PROC_Drive_To_Default",
                                          "PROC_RestoreDefaultSound",
                                          "PROC_SetEepromParameter",
                                          "PROC_HighBusLoad",
                                          "PROC_Start_ExtendedOrDelphiDiagSession",
                                          "PROC_Start_DefaultDiagSession",
                                          "PROC_ChargeSoundRequest"}
              }
            },
            {   // Actions (2)
                {
                    ActionStat::REPLACE_ALL, {"", "{", "}"}
                },
                {
                    ActionStat::SPLIT, {"\\s+"}
                },
                {
                    ActionStat::REMOVE_SPLIT_RESULT_IF_ENDS_WITH, {":"}
                },
                {   // (2): Write "", "=0", "(", "", ">0@,", ");\n"
                    ActionStat::WRITE, {"\t",                                        
                                        "", "T" + Format::cast_target_str(Format::Target::SPLITTED_RAW),
                                        "", "=0",
                                        "(",
                                        "", "@, ",
                                        "", ">0",
                                        ");\n"}
                }
            }
        },
        {   // PROC_E2P_CheckOrRestore
            {   // Conditions (1)
                {   // (1): Starts With "PROC_E2P_CheckOrRestore"
                    ActionStat::STARTS_WITH, {"PROC_E2P_CheckOrRestore"}
                },
            },
            {   // Actions (2)
                {
                    ActionStat::SPLIT, {"\\s+"}
                },
                {
                    ActionStat::CHANGE_PHASE, {QString::number(static_cast<uint>(Phase::PROC_E2P_CheckOrRestore)), QString::number(true)}
                }
            }
        },
        {   // Rule 5: test_result
            {   // Conditions (1)
                {   // (1): Starts With "test_result"
                    ActionStat::STARTS_WITH, {"test_result"}
                },
            },
            {   // Actions (2)
                {
                    ActionStat::SPLIT, {"\\s+"}
                },
                {   // (2): Write "" "=1" "ED_TS(" "" ">1@ " ");\n"
                    ActionStat::WRITE, {"\t",
                                        "", "T" + Format::cast_target_str(Format::Target::SPLITTED_RAW),
                                        "", "=1",
                                        "ED_TS(",
                                        "", "@ ",
                                        "", ">1",
                                        ");\n"}
                }

            }
        },
        {   // Rule 7: Starts With check_signal, go to phase check_signal
            {   // Conditions (3)
                {
                    ActionStat::STARTS_WITH, {"check_signal"}
                }
            },
            {
               // Actions (2)
                {
                    ActionStat::SPLIT, {"\\s+"}
                },
                {
                    ActionStat::CHANGE_PHASE, {QString::number(static_cast<uint>(Phase::CHECK_SIGNAL)), QString::number(true)}
                }

            },
            RuleControl::BREAK_RULE_CHECK_DONT_EXEC_ON_END_ACTIONS
        },
        {   // Rule 8: Starts With set_signal CAN_*
            {   // Conditions (3)
                {
                    ActionStat::STARTS_WITH, {"set_signal"}
                },
            },
            {
               // Actions (3)
                {
                    ActionStat::SPLIT, {"\\s+"}
                },
                {
                    ActionStat::CHANGE_PHASE, {QString::number(static_cast<uint>(Phase::SET_SIGNAL)), QString::number(true)}
                }
            },
            RuleControl::BREAK_RULE_CHECK_DONT_EXEC_ON_END_ACTIONS
        },
        {   // Rule: Remove "puts "Done? y/n""
            {
                {
                    ActionStat::COMPARE, {"1", "puts \"Done? y/n\"",
                                          "4", "", "T" + Format::cast_target_str(Format::Target::RAW),
                                          "", "="}
                }
            },
            {   // No Actions
            }
        },
        {
            {
                {
                    ActionStat::STARTS_WITH, {"set"}
                }
            },
            {
                {
                    ActionStat::FORMAT, {"set"}
                },
                {
                    ActionStat::WRITE, {"// _PH_SET_ ",
                                        "", "T"+Format::cast_target_str(Format::Target::RAW),
                                        "", "=",
                                        "\n"}
                },/* ___
                {
                    ActionStat::SPLIT, {"\\s+"}
                },
                {
                    ActionStat::ADD_VARIABLE, {}
                }*//*
            }
        },
        {
            {
                {
                    ActionStat::STARTS_WITH, {"wait_for_event"}
                }
            },
            {
                {
                    ActionStat::FORMAT, {"wait_for_event"}
                },
                {
                    ActionStat::WRITE, {"// _PH_WAIT_FOR_EVENT_ ",
                                        "", "T"+Format::cast_target_str(Format::Target::RAW),
                                        "", "=",
                                        "\n"}
                }
            }
        },
        {
            {
                {
                    ActionStat::STARTS_WITH, {"start_timer"}
                }
            },
            {
                {
                 ActionStat::FORMAT, {"start_timer"}
                },
                {
                    ActionStat::WRITE, {"// _PH_START_TIMER_ ",
                                        "", "T"+Format::cast_target_str(Format::Target::RAW),
                                        "", "=",
                                        "\n"}
                }
            }
        },
        {
            {
                {
                    ActionStat::STARTS_WITH, {"wait_for_timer"}
                }
            },
            {
                {
                 ActionStat::FORMAT, {"wait_for_timer"}
                },
                {
                    ActionStat::WRITE, {"// _PH_WAIT_FOR_EVENT_ ",
                                        "", "T"+Format::cast_target_str(Format::Target::RAW),
                                        "", "=",
                                        "\n"}
                }
            }
        },
        {
            {
                {
                    ActionStat::STARTS_WITH, {"register_event"}
                }
            },
            {
                {
                 ActionStat::FORMAT, {"register_event"}
                },
                {
                    ActionStat::WRITE, {"// _PH_REGISTER_EVENT_ ",
                                        "", "T"+Format::cast_target_str(Format::Target::RAW),
                                        "", "=",
                                        "\n"}
                }
            }
        },
        {
            {
                {
                    ActionStat::STARTS_WITH, {"check_period"}
                }
            },
            {
                {
                 ActionStat::FORMAT, {"check_period"}
                },
                {
                    ActionStat::WRITE, {"// _PH_CHECK_PERIOD_ ",
                                        "", "T"+Format::cast_target_str(Format::Target::RAW),
                                        "", "=",
                                        "\n"}
                }
            }
        },
        {
            {
                {
                    ActionStat::STARTS_WITH, {"foreach"}
                }
            },
            {
                {
                 ActionStat::FORMAT, {"foreach"}
                },
                {
                    ActionStat::WRITE, {"// _PH_FOREACH_ ",
                                        "", "T"+Format::cast_target_str(Format::Target::RAW),
                                        "", "=",
                                        "\n"}
                }
            }
        },
        {
            {
                {
                    ActionStat::STARTS_WITH, {"check_event"}
                }
            },
            {
                {
                 ActionStat::FORMAT, {"check_event"}
                },
                {
                    ActionStat::WRITE, {"// _PH_CHECK_EVENT_ ",
                                        "", "T"+Format::cast_target_str(Format::Target::RAW),
                                        "", "=",
                                        "\n"}
                }
            }
        },
        {
            {
                {
                    ActionStat::STARTS_WITH, {"put_comment"}
                }
            },
            {
                {
                 ActionStat::FORMAT, {"put_comment"}
                },
                {
                    ActionStat::WRITE, {"// _PH_PUT_COMMENT_ ",
                                        "", "T"+Format::cast_target_str(Format::Target::RAW),
                                        "", "=",
                                        "\n"}
                }
            }
        },
        {
            {
                {
                    ActionStat::STARTS_WITH, {"puts"}
                }
            },
            {
                {
                 ActionStat::FORMAT, {"puts"}
                },
                {
                    ActionStat::WRITE, {"// _PH_PUTS_ ",
                                        "", "T"+Format::cast_target_str(Format::Target::RAW),
                                        "", "=",
                                        "\n"}
                }
            }
        },
        {
            {
                {
                    ActionStat::STARTS_WITH, {"for"}
                }
            },
            {
                {
                 ActionStat::FORMAT, {"for"}
                },
                {
                    ActionStat::WRITE, {"// _PH_FOR_ ",
                                        "", "T"+Format::cast_target_str(Format::Target::RAW),
                                        "", "=",
                                        "\n"}
                }
            }
        },
        {
            {
                {
                    ActionStat::STARTS_WITH, {"eval"}
                }
            },
            {
                {
                 ActionStat::FORMAT, {"eval"}
                },
                {
                    ActionStat::WRITE, {"// _PH_EVAL_ ",
                                        "", "T"+Format::cast_target_str(Format::Target::RAW),
                                        "", "=",
                                        "\n"}
                }
            }
        },
        {
            {
                {
                    ActionStat::STARTS_WITH, {"close"}
                }
            },
            {
                {
                 ActionStat::FORMAT, {"close"}
                },
                {
                    ActionStat::WRITE, {"// _PH_CLOSE_ ",
                                        "", "T"+Format::cast_target_str(Format::Target::RAW),
                                        "", "=",
                                        "\n"}
                }
            }
        },
        {
            {
                {
                    ActionStat::STARTS_WITH, {"continue"}
                }
            },
            {
                {
                 ActionStat::FORMAT, {"continue"}
                },
                {
                    ActionStat::WRITE, {"// _PH_CONTINUE_ ",
                                        "", "T"+Format::cast_target_str(Format::Target::RAW),
                                        "", "=",
                                        "\n"}
                }
            }
        },
        {
            {
                {
                    ActionStat::STARTS_WITH, {"gets"}
                }
            },
            {
                {
                 ActionStat::FORMAT, {"gets"}
                },
                {
                    ActionStat::WRITE, {"// _PH_GETS_ ",
                                        "", "T"+Format::cast_target_str(Format::Target::RAW),
                                        "", "=",
                                        "\n"}
                }
            }
        },
        {
            {
                {
                    ActionStat::STARTS_WITH, {"check_delay"}
                }
            },
            {
                {
                 ActionStat::FORMAT, {"check_delay"}
                },
                {
                    ActionStat::WRITE, {"// _PH_CHECK_DELAY_ ",
                                        "", "T"+Format::cast_target_str(Format::Target::RAW),
                                        "", "=",
                                        "\n"}
                }
            }
        },
        {
            {
                {
                    ActionStat::STARTS_WITH, {"Wave7 Wave8 Wave9"}
                }
            },
            {
                {
                    ActionStat::WRITE, {"//  ",
                                        "", "T"+Format::cast_target_str(Format::Target::RAW),
                                        "", "=",
                                        "\n"}
                }
            }
        },
        {
            {
                {
                    ActionStat::STARTS_WITH, {"{", "["}
                }
            },
            {
                {
                 ActionStat::FORMAT, {"OPEN BRACKETS"}
                },
                {
                    ActionStat::WRITE, {"// _PH_OPEN_BRACKET_ ",
                                        "", "T"+Format::cast_target_str(Format::Target::RAW),
                                        "", "=",
                                        "\n"}
                }
            }
        },
        {
            {
                {
                    ActionStat::ENDS_WITH, {"{"}
                }
            },
            {
                {
                 ActionStat::FORMAT, {"OPEN BRACKET AT END"}
                },
                {
                    ActionStat::WRITE, {"// _PH_OPEN_BRACKET_ ",
                                        "", "T"+Format::cast_target_str(Format::Target::RAW),
                                        "", "=",
                                        "\n"}
                }
            }
        },
        {
            {
                {
                    ActionStat::STARTS_WITH, {"}"}
                }
            },
            {
                {
                 ActionStat::FORMAT, {"CLOSE BRACKET AT START"}
                },
                {
                    ActionStat::WRITE, {"// _PH_CLOSE_BRACKET_ ",
                                        "", "T"+Format::cast_target_str(Format::Target::RAW),
                                        "", "=",
                                        "\n"}
                }
            }
        },
        {   // ALWAYS AT END
            {
                {
                    ActionStat::ENDS_WITH, {"\\"}
                }
            },
            {
                {
                 ActionStat::FORMAT, {"CONTINUE LINE"}
                },
                {
                    ActionStat::WRITE, {"// _PH_CONTINUE_LINE_ ",
                                        "", "T"+Format::cast_target_str(Format::Target::RAW),
                                        "", "=",
                                        "\n"}
                }
            }
        }*/


    },
    {   // On End
        {   // On No Rules
               // Actions (1)
            {   // (1): Write Line
                ActionStat::INTERPRET, {}
            }

        },
        {   // On End of Rules Check

        }
    }
}
;
//,
/*
const FSD_ByLine_TcFileModifierData::Data::ModifierPhase _PHASE_CHECK_SIGNAL__KWP_DIAGNOSTIC =
{
    {
        {   // Rule (1) : DELPHI_DIAG_SESSION
            {   // Conditions (1):
                {   // Compare DELPHI_DIAG_SESSION on (2) argument
                    ActionStat::COMPARE, {"1",
                                          "DELPHI_DIAG_SESSION",
                                          "4", "","T" + Format::cast_target_str(Format::Target::SPLITTED_RAW),
                                          "", "=2"}
                }
            },
            {   // Actions (1):               
                {   // (1) : Write DelphiSession function call (_PH_ COMMENTED)
                    ActionStat::WRITE, {"\t//-------------------------\n\t// ",
                                        "","T" + Format::cast_target_str(Format::Target::RAW),
                                        "", "=",
                                        "\n\t// VVV\n\t// _PH_ TsSendDiagRequest(StartDelphiSessionReq);\n\t// -----------------------\n"}
                }
            }
        },
        {   // Rule (2) : DEFAULT_DIAG_SESSION
            {   // Conditions (1):
                {   // Compare DEFAULT_DIAG_SESSION on (2) argument
                    ActionStat::COMPARE, {"1",
                                          "DEFAULT_DIAG_SESSION",
                                          "4", "","T" + Format::cast_target_str(Format::Target::SPLITTED_RAW),
                                          "", "=2"}
                }
            },
            {   // Actions (1):
                {   // (1) : Write DefaultSession function call (_PH_ COMMENTED)
                    ActionStat::WRITE, {"\t//-------------------------\n\t// ",
                                        "","T" + Format::cast_target_str(Format::Target::RAW),
                                        "", "=",
                                        "\n\t// VVV\n\t// _PH_ TsSendDiagRequest(StartDefaultSessionReq);\n\t// -----------------------\n"}
                }
            }
        },
        {   // Rule (2) : EXTENDED_DIAG_SESSION
            {   // Conditions (1):
                {   // Compare EXTENDED_DIAG_SESSION on (2) argument
                    ActionStat::COMPARE, {"1",
                                          "EXTENDED_DIAG_SESSION",
                                          "4", "","T" + Format::cast_target_str(Format::Target::SPLITTED_RAW),
                                          "", "=2"}
                }
            },
            {   // Actions (1):
                {   // (1) : Write ExtendedSession function call (_PH_ COMMENTED)
                    ActionStat::WRITE, {"\t//-------------------------\n\t// ",
                                        "","T" + Format::cast_target_str(Format::Target::RAW),
                                        "", "=",
                                        "\n\t// VVV\n\t// _PH_ TsSendDiagRequest(StartExtendedSessionReq);\n\t// -----------------------\n"}
                }
            }
        }
    },
    {   // On End
        {   // On No Rules
               // Actions (1)

            {
             ActionStat::FORMAT, {"CHECK_SIGNAL_KWP"}
            },
            {   // (1): Write Line
                ActionStat::WRITE, {"\t// _PH_CHECK_SIGNAL_KWP_ ",
                                    "","T" + Format::cast_target_str(Format::Target::RAW),
                                    "", "=",
                                    "\n"}
            }
        },
        {   // On End of Rules Check
               // Actions (1): Back to Standard Phase
            { // Action to Standard Phase
              ActionStat::CHANGE_PHASE, {QString::number(static_cast<uint>(Phase::STANDARD)), QString::number(false)}
            }

        },

    }
}
;
const FSD_ByLine_TcFileModifierData::Data::ModifierPhase _PHASE_PROC_E2P_CheckOrRestore = {
    {   // Rules
        {   // Rule 1: RESTORE
            {   // Condtions : RESTORE || CHECK
                {
                    ActionStat::COMPARE, {"2",
                                          "RESTORE",
                                          "CHECK",
                                          "4", "","T" + Format::cast_target_str(Format::Target::SPLITTED_RAW),
                                          "", "=1"}
                }
            },
            {   // Actions Write "\tPROC_E2P_CheckOrRestore(E2P_" "" "=1" ");\n"
                {
                    ActionStat::WRITE, {"\tPROC_E2P_CheckOrRestore(E2P_",
                                        "","T" + Format::cast_target_str(Format::Target::SPLITTED_RAW),
                                        "", "=1",
                                        ");\n"}
                }
            }
        },
    },
    {   // On End
        {   // On No Rules
            // Actions (1)
            {   // (1): Write Line
                ActionStat::WRITE, {"\t",
                                    "","T" + Format::cast_target_str(Format::Target::RAW),
                                    "", "=",
                                    "\n"}
            }
        },
        {   // On End of Rules Check
           // Actions (1): Back to Standard Phase
            { // Action to Standard Phase
              ActionStat::CHANGE_PHASE, {QString::number(static_cast<uint>(Phase::STANDARD)), QString::number(false)}
            }

        },
    }
};

const FSD_ByLine_TcFileModifierData::Data::ModifierPhase _PHASE_CHECK_SIGNAL = {
    {   // Rules
        {   // Rule 1:
            {   // Condtions : AMPLI | SOUND
                {
                    ActionStat::COMPARE, {"2",
                                          "mVSP_AMPLI_STATUS",
                                          "mVSP_SOUND_STATUS",
                                          "4",
                                          "", "T" + Format::cast_target_str(Format::Target::SPLITTED_RAW),
                                          "", "=1"}
                }
            },
            {   // Actions
                {   // (1) : Write "if(" "" "=1" "== `ON){\nPASSED_TS(\"" "" "=1" "is ON.\");\n}else{\nFAILED_TS(\"" "" "=1" ");\n}\n"
                    ActionStat::WRITE, {"\tif(",
                                        "","T" + Format::cast_target_str(Format::Target::SPLITTED_RAW),
                                        "", "=1",
                                        "() == `ON){\n\t\tPASSED_TS(\"",
                                        "", "=1",
                                        " is ON.\");\n\t}else{\n\t\tFAILED_TS(\"",
                                        "", "=1",
                                        " is OFF.\");\n\t}\n"}
                }
            }
        },
        {   // Rule 2 : KWP_SDS
            {   // Conditions
                {
                    ActionStat::COMPARE, {"1",
                                          "KWP_SDS",
                                          "4",
                                          "", "T" + Format::cast_target_str(Format::Target::SPLITTED_RAW),
                                          "", "=1"}
                }
            },
            {
                {   // (1): Change Phase KWP
                    ActionStat::CHANGE_PHASE, {QString::number(static_cast<uint>(Phase::CHECK_SIGNAL__KWP_DIAGNOSTIC)), QString::number(true)}
                }
            },
            RuleControl::BREAK_RULE_CHECK_DONT_EXEC_ON_END_ACTIONS
        }
    },
    {   // On End
        {   // On No Rules
           // Actions (1)

            {
             ActionStat::FORMAT, {"CHECK_SIGNAL"}
            },
            {   // (1): Write Line
                ActionStat::WRITE, {"\t// _PH_CHECK_SIGNAL_",
                                    "","T" + Format::cast_target_str(Format::Target::RAW),
                                    "", "=",
                                    "\n"}
            }

        },
        {   // On End of Rules Check
           // Actions (1): Back to Standard Phase
            { // Action to Standard Phase
              ActionStat::CHANGE_PHASE, {QString::number(static_cast<uint>(Phase::STANDARD)), QString::number(false)}
            }

        },
    }
};

const FSD_ByLine_TcFileModifierData::Data::ModifierPhase _PHASE_SET_SIGNAL = {
    {   // Rules
        {   // Rule 1: RESTORE
            {   // Condtions : RESTORE || CHECK                
                /*{
                    ActionStat::REPLACE_ALL, {"", "\""}
                },*//*
                {
                    ActionStat::SPLIT, {"\\s+"}
                },
                {
                    ActionStat::COMPARE, {"6",
                                          "CAN_PowerTrainStatus",
                                          "CAN_ATCVT_RangeIndicationExtended",
                                          "CAN_PeriodicReverseBuzzerTrigger",
                                          "CAN_BrakeInfoStatus",
                                          "CAN_GADE",
                                          "CAN_StopStartInformationDisplay",
                                          "2", "", "=1"}
                },
                {
                    ActionStat::GET_MSG_BY_SIG_NAME, {"4", "", "\\1;" + QString::number(QStringLiteral("CAN_").length()),
                                                      "", "=1",
                                                      "2", "", "=2"
                                                      }
                }
            },
            {   // Actions Write "\tPROC_E2P_CheckOrRestore(E2P_" "" "=1" ");\n"                

                {   // (1) : SSTR Write "$" "" "=SSTR" " = " "" "=2" ";\n"
                    ActionStat::WRITE, {"\t$",
                                        "", "=1",
                                        " = VtSig_",
                                        "", "=1",
                                        "::",
                                        "", "=2",
                                        ";\n"}
                }
            }
        },
        {
            {
                {
                    ActionStat::COMPARE, {"6",
                                          "PowerTrainStatus",
                                          "ATCVT_RangeIndicationExtended",
                                          "PeriodicReverseBuzzerTrigger",
                                          "BrakeInfoStatus",
                                          "GADE",
                                          "StopStartInformationDisplay",
                                          "2", "", "=1"}

                }
            },
            {
                {
                    ActionStat::REPLACE_ALL, {"", "\""}
                },
                {
                    ActionStat::SPLIT, {"\\s+"}
                },
                {
                    ActionStat::WRITE, {"\t$",
                                        "", "\\1;" + QString::number(QStringLiteral("CAN_").length()),
                                        "", "=1",
                                        " = ",
                                        "", "@ ",
                                        "", ">1",
                                        ";\n"}
                }
            }
        },
        {   // Rule 1: RESTORE
            {   // Condtions : RESTORE || CHECK
                {
                    ActionStat::COMPARE, {"2",
                                          "CAN_VehicleSpeed",
                                          "CAN_EngineRPM",
                                           "6", "","T" + Format::cast_target_str(Format::Target::SPLITTED_RAW),
                                          "", "\\1;" + QString::number(QStringLiteral("CAN_").length()),
                                          "", "=1"}
                },                
            },
            {   // Actions Write "\tPROC_E2P_CheckOrRestore(E2P_" "" "=1" ");\n"

                {   // (1) : SSTR Write "$" "" "=SSTR" " = " "" "=2" ";\n"
                    ActionStat::WRITE, {"\t$",
                                        "", "T" + Format::cast_target_str(Format::Target::SPLITTED_RAW),
                                        "", "=1",
                                        " = ",                                        
                                        "", "T" + Format::cast_target_str(Format::Target::SPLITTED_RAW),
                                        "", "=2",
                                        ";\n"}
                }
            }
        },
        {   // Rule 2: VSP_SPEAKER_FAIL
            {   // Conditions
                {
                    ActionStat::COMPARE, {"1",
                                          "VSP_SPEAKER_FAIL",
                                          "4","", "T" + Format::cast_target_str(Format::Target::SPLITTED_RAW),
                                          "", "=1"}
                }
            },
            {   // Actions
                {
                    ActionStat::WRITE, {"\t//-------------------------\n\t// ",
                                        "", "T" + Format::cast_target_str(Format::Target::RAW),
                                        "", "=",
                                        "\n\t// VVV\n\tBC_Set_Signal(\"",
                                        "", "T" + Format::cast_target_str(Format::Target::SPLITTED_RAW),
                                        "", "=1",
                                        "\", \"",
                                        "", "=2",
                                        "\");\n\t// -----------------------\n"}
                }
            }
        }
    },
    {   // On End
        {   // On No Rules
               // Actions (1)
            {
             ActionStat::FORMAT, {"SET_SIGNAL"}
            },
            {   // (1): Write Line
                ActionStat::WRITE, {"\t// _PH_SET_SIGNAL_ ",
                                    "", "T" + Format::cast_target_str(Format::Target::RAW),
                                    "", "=",
                                    "\n"}
            }

        },
        {   // On End of Rules Check
           // Actions (1): Back to Standard Phase
            { // Action to Standard Phase
              ActionStat::CHANGE_PHASE, {QString::number(static_cast<uint>(Phase::STANDARD)), QString::number(false)}
            }

        },
    }
};

const FSD_ByLine_TcFileModifierData::Data::ModifierPhase _PHASE_CALL = {
    {   // Rules
        {    // Rule : Generate DTC
             {  // Conditions
                {
                    ActionStat::COMPARE, {"1",
                                          "GenerateDTC",
                                          "2",
                                          "", "=1"}
                },
                {   // Replace with "" strings like "{", "}"
                    ActionStat::REPLACE_ALL, {"", "{", "}"}
                },
                {
                    ActionStat::SPLIT, {"\\s+"}
                },
                {
                    ActionStat::REMOVE_SPLIT_RESULT_IF_ENDS_WITH, {":"}
                },
                {
                    ActionStat::COMPARE_REGEX, {"1", RegExpCore::regExpForHexString,
                                                "2", "", "=2"}
                }
             },
             {  // Actions
                {   // Write ""
                    ActionStat::WRITE, {"\t//-------------------------\n\t// ",
                                        "", "T" + Format::cast_target_str(Format::Target::RAW),
                                        "", "=",
                                        "\n\t// VVV\n\t",
                                        "", "T" + Format::cast_target_str(Format::Target::SPLITTED_RAW),
                                        "", "=1",
                                        "(0x",
                                        "", "@, 0x",
                                        "", "/2;2",
                                        ");\n\t// -----------------------\n"}
                }
            }
        },
        {    // Rule : CheckIfDTCPresent
             {  // Conditions
                {
                    ActionStat::COMPARE, {"1",
                                          "CheckIfDTCPresent",
                                          "2",
                                          "", "=1"}
                },
             },
             {  // Actions
                {   // Replace with "" strings like "{", "}"
                    ActionStat::REPLACE_ALL, {"", "{", "}"}
                },
                {
                    ActionStat::SPLIT, {"\\s+"}
                },
                {
                    ActionStat::REMOVE_SPLIT_RESULT_IF_ENDS_WITH, {":"}
                },
                {   // Write ""
                    ActionStat::WRITE, {"\t//-------------------------\n\t// ",
                                        "", "T" + Format::cast_target_str(Format::Target::RAW),
                                        "", "=",
                                        "\n\t// VVV\n\t",
                                        "", "T" + Format::cast_target_str(Format::Target::SPLITTED_RAW),
                                        "", "=1",
                                        "(0x",
                                        "", "=2",
                                        ", 0x",
                                        "", "@, 0x",
                                        "", "C2;2",
                                        ");\n\t// -----------------------\n"}
                }
            }
        },
    },
    {   // On End
        {   // On No Rules
           // Actions (1)
            {   // Replace with "" strings like "{", "}"
                ActionStat::REPLACE_ALL, {"", "{", "}"}
            },
            {
                ActionStat::SPLIT, {"\\s+"}
            },
            {
                ActionStat::REMOVE_SPLIT_RESULT_IF_ENDS_WITH, {":"}
            },
            {   // (1): Write "", "=1", "(", "", ">1@,", ");\n"
                ActionStat::WRITE, {"\t",
                                    "", "T" + Format::cast_target_str(Format::Target::SPLITTED_RAW),
                                    "", "=1",
                                    "(",
                                    "", "@,",
                                    "", ">1",
                                    ");\n"}
            }

        },
        {   // On End of Rules Check
           // Actions (1): Back to Standard Phase
            { // Action to Standard Phase
              ActionStat::CHANGE_PHASE, {QString::number(static_cast<uint>(Phase::STANDARD)), QString::number(false)}
            }

        },
    }
};

const FSD_ByLine_TcFileModifierData::Data::ModifierPhase _PHASE_IF = {
    {   // Rules
        {
            {   // On any "file"
               {
                    ActionStat::CONTAINS, {"file"}
               }
            },
            {   // Comment
                {
                 ActionStat::FORMAT, {"file"}
                },
                {
                    ActionStat::WRITE, {"// _PH_FILE_FUNCTION_ ",
                                        "", "T" + Format::cast_target_str(Format::Target::RAW),
                                        "", "=", "\n"}
                }
            }
        },/*
        {
            { // on "{" "$result" "!=" "0" "}" "{" "test_result"              
              {
                  ActionStat::COMPARE, {"1", "{", "2", "", "=1"}
              },
              {
                  ActionStat::COMPARE, {"2",
                                        "$result",
                                        "$evt_value",
                                        "2", "", "=2"}
              },
              {
                  ActionStat::COMPARE, {"1", "!=", "2", "", "=3"}
              },
              {
                  ActionStat::COMPARE, {"1", "}", "2", "", "=5"}
              },
              {
                  ActionStat::COMPARE, {"1", "{test_result", "2", "", "=6"}
              },
            },
            {
                {
                    ActionStat::REPLACE_ALL, {"", "{", "}"}
                },
                {
                    ActionStat::SPLIT, {"\\s+"}
                },
                {
                    ActionStat::WRITE, {"\tif(result != 0){\n\t", "", "=5", "ED_TS(\"\");\n\t}\n"}
                }
            }
        },*/
        /*{
            {   // on GetDTCStatusByte
                {
                    ActionStat::COMPARE, {"1", "{[call", "2", "", "=1"}
                },
                {
                    ActionStat::COMPARE, {"1", "GetDTCStatusByte", "2", "", "=2"}
                },
                {
                    ActionStat::COMPARE, {"1", "{DTCCode:", "2", "", "=3"}
                },
                {
                    ActionStat::COMPARE, {"1", "!=", "2", "", "=5"}
                },
                {
                    ActionStat::COMPARE, {"1", "{test_result", "2", "", "=7"}
                }
            },
            {
                {
                    ActionStat::REPLACE_ALL, {"", "{", "}", "[", "]"}
                },
                {
                    ActionStat::SPLIT, {"\\s+"}
                },
                {
                    ActionStat::SSTR_SAVE, {"", "=6"}
                },
                {
                    ActionStat::WRITE, {"\t// ------------------------------\n\t//",
                                        "", "T" + Format::cast_target_str(Format::Target::RAW),
                                        "", "=", "\n\t// VVV\n\tif(GetDTCStatusByte(0x",
                                        "", "T" + Format::cast_target_str(Format::Target::SPLITTED_RAW),
                                        "", "@, 0x",
                                        "", "/4;2", ") != 0x",
                                        "", "T" + Format::cast_target_str(Format::Target::SSTR),
                                        "", "<>1;-1",
                                        "", "T" + Format::cast_target_str(Format::Target::SPLITTED_RAW),
                                        "){\n\t\t",
                                        "", "=8",
                                        "ED_TS(",
                                        "", "@ ",
                                        "", ">8", ");\n\t}\n\t// -------------------------\n"}
                },
            },

        },
        {
            {
               {
                    ActionStat::COMPARE, {"1",
                                          "if {[string compare -nocase [gets stdin] y]!=0} {test_result FAIL}",
                                          "4", "", "T" + Format::cast_target_str(Format::Target::RAW),
                                          "", "="}
                },
            },
            {
                {
                    ActionStat::WRITE, {"\tresult = TestWaitForTesterConfirmation(\"\", 60000);  // 1 min for answear\n\
                                        switch(result){\n\
                                          case 1: // Yes\n\
                                            PASSED_TS(\"\");\n\
                                            break;\n\
                                          case 2: // No\n\
                                            FAILED_TS(\"\");\n\
                                           break;\n\
                                          default:\n\
                                            WARNING_TS(\"Timeout or error occured.\");\n\
                                            break;\n\
                                        }\n"}
                }
            }
        },
        {
            {   // Rule: Change if { [string compare -nocase [gets stdin] y]==0 } {
                {
                    ActionStat::COMPARE, {"1", "if { [string compare -nocase [gets stdin] y]==0 } {",
                                          "4", "", "T" + Format::cast_target_str(Format::Target::RAW),
                                          "", "="}
                }
            },
            {
                {
                    ActionStat::WRITE, {"\tresult = TestWaitForTesterConfirmation(\"\", 60000);  // 1 min for answear\n\
                                        switch(result){\n\
                                          case 1: // Yes\n\
                                            PASSED_TS(\"Global variables have been updated.\");\n\
                                            break;\n\
                                          case 2: // No\n\
                                            FAILED_TS(\"Global variables haven't been updated.\");\n\
                                           break;\n\
                                          default:\n\
                                            WARNING_TS(\"Timeout or error occured.\");\n\
                                            break;\n\
                                        }\n"}
                }
            }
        }
    },
    {   // On End
        {   // On No Rules
               // Actions (1)
            {
                ActionStat::FORMAT, {"if"}
            },
            {   // (1): Write "", "=1", "(", "", ">1@,", ");\n"
                ActionStat::WRITE, {"// _PH_IF_ ",
                                    "", "T" + Format::cast_target_str(Format::Target::RAW),
                                    "", "=",
                                    "\n"}
            }

        },
        {   // On End of Rules Check
           // Actions (1): Back to Standard Phase

            { // Action to Standard Phase
              ActionStat::CHANGE_PHASE, {QString::number(static_cast<uint>(Phase::STANDARD)), QString::number(false)}
            }

        },*//*
    }
};

const FSD_ByLine_TcFileModifierData::Data::ModifierPhase _PHASE_IGNORE_UNTIL_END_OF_BLOCK = {
    {   // Rules
        {
            {
                {
                    ActionStat::STARTS_WITH, {"}"}
                }
            },
            {
                {
                    ActionStat::CHANGE_PHASE, {QString::number(static_cast<uint>(Phase::STANDARD)), QString::number(false)}
                }
            }
        }
    },
    {   // On End
        {   // On No Rules
               // Actions (1)

        },
        {   // On End of Rules Check
           // Actions (1): Back to Standard Phase

        },
    }
};*/
// --------------------------------------------------
/*
const ModifierPhases FSD_ByLine_TcFileModifierData::Data::phases =
{
    _PHASE_TEST_CASE_INFO,
    _PHASE_STANDARD,/*
    _PHASE_CHECK_SIGNAL,
    _PHASE_CHECK_SIGNAL__KWP_DIAGNOSTIC,
    _PHASE_SET_SIGNAL,
    _PHASE_PROC_E2P_CheckOrRestore,
    _PHASE_CALL,
    _PHASE_IF,
    _PHASE_IGNORE_UNTIL_END_OF_BLOCK
};*/

bool FSD_ByLine_TcFileModifierData::Data::toDefaultPhase(){
    curPhase = userConfig_.modifierPhases().constFind("Default");
    return curPhase != userConfig_.modifierPhases().constEnd();
}

template<>
bool FSD_ByLine_TcFileModifierData::Config::initialize(){
    interpreterData = new FSD_ByLine_TcFileModifierData::Data(dataModel.userConfig(), dataModel.caplFunctionDefinitionsRef());
    data = new FileReaderInterpreter_Configuration_Priv<FSD_ByLine>::Data;
    if(not FRI_COM_Manager::next(COMChannel)) // Get Data Segment
        return ERROR_CALL("Internal Error: End of COMChannelData");
    NS_FRI_COM_Manager::ChannelData& dataCmd = FRI_COM_Manager::receive(COMChannel);
    if(dataCmd.command != NS_FRI_COM_Manager::Commands::DATA || !data)
        return ERROR_CALL("Internal Error: Wrong COM Command");
    interpreterData->fileDir = QDir(*static_cast<QString*>(dataCmd.channelData));
    interpreterData->fileDir.cdUp();
    interpreterData->fileDir.cdUp();
    if(interpreterData->toDefaultPhase() == false){ // Failed
        return ERROR_CALL("Internal Error: Initialization Failed - No Default Phase");
    }
    switch(dataModel.initialize()){
    case NewDataModel::InitializeStatus::INITIALIZE_SUCCESS:
    {

        ControllerConfigInfo::Attributes::Iterator processStatusAttr = interpreterData->userConfig_.attributes().find("MODEL_PROCESS_STATUS");
        if(processStatusAttr == interpreterData->userConfig_.attributes().end())
        {
            interpreterData->tclToCaplInterpreter_.addIgnoreMessage("CRITICAL ERROR: prefix attribute not found.");
            return ERROR_CALL("prefix attributes Critical Error");
        }
        processStatusAttr.value().value = "STARTED";
       interpreterData->userConfig_.attributes().insert("DIR_NAME", ControllerConfigInfo::Attributes::mapped_type{interpreterData->fileDir.dirName()});
//        dataModel.write("/*@!Encoding:1250*/\n"
//"/*-------------------------------------------------------------------------------------------\n"
//"\n"
//"Project:     S71    \n"
//"Written by: \n"
//"\n"
//"Additional information:\n"
//"This file contains tests of " + dataModel.dir().dirName() + " for S71.\n"
//"\n"
//"---------------------------------------------------------------------------------------------\n"
//"Rev | Date        |Author           |Description"
//"--------------------------------------------------------------------------------------------- \n"
//"001 | 30/11/2020  |              |File has been created.\n"
//"\n"
//"-------------------------------------------------------------------------------------------*/\n"
//"\n"
//"includes{\n"
//"//    #include\"../../Libraries/Procedures/NewLib/S71_C1A_Common.cin\"\n"
//"}\n"
//"\n"
//"variables{\n"
//" \n"
//"}\n");
    }
    case NewDataModel::InitializeStatus::ALREADY_INITIALIZED:
    {
        ControllerConfigInfo::Attributes::ConstIterator prefixStr = interpreterData->userConfig_.attributes().constFind("PREFIX_TEXT_FOR_FILE");
        if(prefixStr == interpreterData->userConfig_.attributes().constEnd())
        {
            interpreterData->tclToCaplInterpreter_.addIgnoreMessage("CRITICAL ERROR: prefix attribute not found.");
            return ERROR_CALL("prefix attributes Critical Error");
        }
        dataModel.write(prefixStr.value().value);
    }
        break;
    case NewDataModel::InitializeStatus::INITIALIZE_FAIL:
        return ERROR_CALL("Internal Error: Initialization Failed");
    }
    return true;
}

template<>
bool FSD_ByLine_TcFileModifierData::Config::deinitialize(){
    if(interpreterData)
    {
        if(interpreterData->tclToCaplInterpreter_.deinitialize() == Core::Error::Error){
            interpreterData->tclToCaplInterpreter_.addIgnoreMessage("CRITICAL ERROR: File is not interpreted by TCL Interpreter after previous TCL interpreter error.");
            return config.ERROR_CALL("TCL Interpreter Deinitialize Critical Error");
        }
        interpreterData->tclToCaplInterpreter_.printErrorReport(dataModel.reportFile(), dataModel.currentTCLFileName());
        dataModel.setTestCaseErrors(interpreterData->tclToCaplInterpreter_.getErrorsNumber());
        dataModel.predefinitions().append(interpreterData->tclToCaplInterpreter_.predefinitions());
        ControllerConfigInfo::Attributes::ConstIterator postfixStr = interpreterData->userConfig_.attributes().constFind("POSTFIX_TEXT_FOR_FILE");
        if(postfixStr == interpreterData->userConfig_.attributes().constEnd())
        {
            interpreterData->tclToCaplInterpreter_.addIgnoreMessage("CRITICAL ERROR: postfix attribute not found.");
            return config.ERROR_CALL("postfix attributes Critical Error");
        }
        config.dataModel.write(interpreterData->tclToCaplInterpreter_.readCommand().replace(QRegularExpression("\n(?=\\S*)(?!\\S*}\\Z)"), "\n\t") + postfixStr.value().value);
        //interpreterData->writeTCInfo(config.dataModel);
    }
    delete data;
    data = nullptr;
    delete interpreterData;
    interpreterData = nullptr;
    return true;
}

template<>template<>template<>
bool FSD_ByLine_TcFileModifierData::processingFunction<FSD_ByLine_TcFileModifierData::Stat::REPLACE_BY_MAPPING>(){
    using ModifierPhases = ModifierPhases;
    using ModifierPhase = ModifierPhases::mapped_type;
    using ModifierRules = ModifierRules;
    using ModifierRule = ModifierRules::value_type;
    bool ruleCondtionsPassed = false;
    ModifierPhases::ConstIterator phase = interpreterData->curPhase;
    for(ModifierRules::ConstIterator rule = phase->rules.constBegin(); rule < phase->rules.constEnd(); rule++){
        ModifierRule::Actions::ConstIterator condition;
        for(condition = rule->conditions.constBegin(); condition < rule->conditions.constEnd(); condition++){
            interpreterData->arguments = condition->arguments;
            if(!(config.*(config.processingFunctions.at(static_cast<int>(Stat::PROCESSING_ACTION_START) + static_cast<int>(condition->action))))())
                return false;// config.ERROR_CALL(Error::ERROR); // Error already should be known
            if(!interpreterData->conditionResult)
                break;
        }
        if(condition == rule->conditions.constEnd()){
            ruleCondtionsPassed = true;
            for(ModifierRule::Actions::ConstIterator action = rule->actions.constBegin(); action < rule->actions.constEnd(); action++){
                interpreterData->arguments = action->arguments;
                if(!(config.*(config.processingFunctions.at(static_cast<int>(Stat::PROCESSING_ACTION_START) + static_cast<int>(action->action))))()){
                    return false;// config.ERROR_CALL(Error::ERROR); // Error already should be known
                }
            }
            switch(rule->controlFlag){
            case RuleControl::BREAK_RULE_CHECK_DONT_EXEC_ON_END_ACTIONS:
                return true;
            case RuleControl::BREAK_RULE_CHECK:
                rule = phase->rules.constEnd();
                break;
            case RuleControl::NO_BREAK_RULE_CHECK:
                ruleCondtionsPassed = false;
                break;
            default:
                return config.ERROR_CALL("Internal Error: Unknown Control Flag in ReplaceByMapping");
            }
        }
    }
    if(!ruleCondtionsPassed){
        for(ModifierRule::Actions::ConstIterator action = phase->onEnd.onNoRules.constBegin(); action < phase->onEnd.onNoRules.constEnd(); action++)
        {
            interpreterData->arguments = action->arguments;
            if(!(config.*(config.processingFunctions.at(static_cast<int>(Stat::PROCESSING_ACTION_START) + static_cast<int>(action->action))))()){
                return false;// config.ERROR_CALL(Error::ERROR); // Error already should be known
            }
        }
    }
    for(ModifierRule::Actions::ConstIterator action = phase->onEnd.onEndOfRulesCheck.constBegin(); action < phase->onEnd.onEndOfRulesCheck.constEnd(); action++)
    {
        interpreterData->arguments = action->arguments;
        if(!(config.*(config.processingFunctions.at(static_cast<int>(Stat::PROCESSING_ACTION_START) + static_cast<int>(action->action))))()){
            return false;// config.ERROR_CALL(Error::ERROR); // Error already should be known
        }
    }
    return true;
}

template<>template<>template<>
bool FSD_ByLine_TcFileModifierData::processingFunction<FSD_ByLine_TcFileModifierData::Stat::ACTION_STARTS_WITH>(){
    const QString PRE_ERROR_MSG = "Internal Error: Action StartsWith";
    if(interpreterData->arguments.size() < 1)
        return config.ERROR_CALL(PRE_ERROR_MSG + " - Wrong Numb Of Action Arguments");
    for(QStringList::ConstIterator str = interpreterData->arguments.constBegin(); str < interpreterData->arguments.constEnd(); str++)
        if((interpreterData->conditionResult = interpreterData->lineData.startsWith(*str)))
            break;
    return true;
}
/*
template<>template<>template<>
bool FSD_ByLine_TcFileModifierData::processingFunction<FSD_ByLine_TcFileModifierData::Stat::ACTION_ENDS_WITH>(){
    if(interpreterData->arguments.size() < 1)
        return config.ERROR_CALL(Error::ERROR);
    for(QStringList::ConstIterator str = interpreterData->arguments.constBegin(); str < interpreterData->arguments.constEnd(); str++)
        if((interpreterData->conditionResult = interpreterData->lineData.endsWith(*str)))
            return true;
    return true;
}
*/
template<>template<>template<>
bool FSD_ByLine_TcFileModifierData::processingFunction<FSD_ByLine_TcFileModifierData::Stat::ACTION_WRITE_TO_TC_INFO>(){
    using TCInfoData = TC_Info_Data;
    const QString PRE_ERROR_MSG = "Internal Error: Action WriteToTcInfo";
    TCInfoData tcDataSelector = TCInfoData::SIZE;
    QString str;
    {   // Preverification
        bool ok = false;
        if(interpreterData->arguments.size() < 2 || // Required: at least 2 arguments. 1st as tcDataSelector, 2nd and so on as data
                (tcDataSelector = static_cast<TCInfoData>(interpreterData->arguments.at(0).toUInt(&ok)), !ok) || tcDataSelector >= TCInfoData::SIZE    // Selector in enum range and conversion success
                ){
            return config.ERROR_CALL(PRE_ERROR_MSG);
        }
    }
    switch(tcDataSelector){
    case TCInfoData::NAME:
        if(!interpreterData->createAndAssignString(interpreterData->tcData.name, interpreterData->arguments.mid(1)))
            return config.ERROR_CALL(PRE_ERROR_MSG + " - Write to Name");
        break;
    case TCInfoData::AUTHOR:
        if(!interpreterData->createAndAssignString(interpreterData->tcData.author, interpreterData->arguments.mid(1)))
            return config.ERROR_CALL(PRE_ERROR_MSG + " - Write to Author");
        break;
    case TCInfoData::DESCRIPTION:
        if(!interpreterData->createAndAssignString(interpreterData->tcData.description, interpreterData->arguments.mid(1)))
            return config.ERROR_CALL(PRE_ERROR_MSG + " - Write to Description");
        break;
    case TCInfoData::DOCUMENTS:
        if(!interpreterData->createAndAssignString(str, interpreterData->arguments.mid(1)))
            return config.ERROR_CALL(PRE_ERROR_MSG + " - Write to Documents");
        if(!interpreterData->tcData.documents.isEmpty())
            interpreterData->tcData.documents += QStringLiteral("; ") + str;
        else
            interpreterData->tcData.documents = str;
        break;
    case TCInfoData::INTEGRATION:
        if(!interpreterData->createAndAssignString(interpreterData->tcData.integration, interpreterData->arguments.mid(1)))
            return config.ERROR_CALL(PRE_ERROR_MSG + " - Write to Integration");
        break;
    case TCInfoData::REGRESSION:
        if(!interpreterData->createAndAssignString(interpreterData->tcData.regression, interpreterData->arguments.mid(1)))
            return config.ERROR_CALL(PRE_ERROR_MSG + " - Write to Regression");
        break;
    case TCInfoData::REQUIREMENTS:        
        if(!interpreterData->createAndAssignString(str, interpreterData->arguments.mid(1)))
            return config.ERROR_CALL(PRE_ERROR_MSG + " - Write to Requirements");
        if(!interpreterData->tcData.requirements.isEmpty())
            interpreterData->tcData.requirements += QStringLiteral("; ") + str;
        else
            interpreterData->tcData.requirements = str;
        break;
    case TCInfoData::TC_DOMAIN:
        if(!interpreterData->createAndAssignString(interpreterData->tcData.domain, interpreterData->arguments.mid(1)))
            return config.ERROR_CALL(PRE_ERROR_MSG + " - Write to TcDomain");
        break;
    case TCInfoData::TITLE:
        if(!interpreterData->createAndAssignString(interpreterData->tcData.title, interpreterData->arguments.mid(1)))
            return config.ERROR_CALL(PRE_ERROR_MSG + " - Write to Title");
        break;
    case TCInfoData::TYPE:
        if(!interpreterData->createAndAssignString(interpreterData->tcData.type, interpreterData->arguments.mid(1)))
            return config.ERROR_CALL(PRE_ERROR_MSG + " - Write to Type");
        break;
    case TCInfoData::VERSION:
        if(!interpreterData->createAndAssignString(interpreterData->tcData.version, interpreterData->arguments.mid(1)))
            return config.ERROR_CALL(PRE_ERROR_MSG + " - Write to Version");
        break;
    }
    return true;
}

template<>template<>template<>
bool FSD_ByLine_TcFileModifierData::processingFunction<FSD_ByLine_TcFileModifierData::Stat::ACTION_WRITE>(){
    const QString PRE_ERROR_MSG = "Internal Error: Action Write";
    QStringList& parameters = interpreterData->arguments;
    QStringList::size_type result = 0;
    QString str;
    if(interpreterData->createAndAssignString(str, interpreterData->arguments) != interpreterData->arguments.size())
        return config.ERROR_CALL(PRE_ERROR_MSG);
    config.dataModel.write(str);
    return true;
}

template<>template<>template<>
bool FSD_ByLine_TcFileModifierData::processingFunction<FSD_ByLine_TcFileModifierData::Stat::ACTION_COMPARE>(){
    const QString PRE_ERROR_MSG = "Internal Error: Action Compare";
    QStringList& parameters = interpreterData->arguments;
    QStringList::size_type result = 0;
    QString formatStr;
    bool ok = false;
    uint numbOfCompares = UINT_MAX;
    uint numbOfFormatArgs = UINT_MAX;

    if(interpreterData->arguments.size() < 2)
        return config.ERROR_CALL(PRE_ERROR_MSG + " - Wrong number of action arguments");
    if((numbOfCompares = interpreterData->arguments.at(0).toUInt(&ok), !ok) ||
            numbOfCompares + 1 >= interpreterData->arguments.size()  ||
            (numbOfFormatArgs = interpreterData->arguments.at(numbOfCompares + 1).toUInt(&ok), !ok) ||
            numbOfCompares + numbOfFormatArgs + 2 != interpreterData->arguments.size())
        return config.ERROR_CALL(PRE_ERROR_MSG + " - Wrong arguments format");
    QStringList&& params = interpreterData->arguments.mid(2 + numbOfCompares);
    if(interpreterData->createAndAssignString(formatStr, params) != params.size())
        return config.ERROR_CALL(PRE_ERROR_MSG + " - Create Text Error");
    for(QStringList::ConstIterator str = interpreterData->arguments.begin() + 1; str < interpreterData->arguments.begin() + 1 + numbOfCompares; str++)
        if((interpreterData->conditionResult = (*str == formatStr)))
            break;
    return true;
}
/*
template<>template<>template<>
bool FSD_ByLine_TcFileModifierData::processingFunction<FSD_ByLine_TcFileModifierData::Stat::ACTION_COMPARE_REGEX>(){
    QString formatStr;
    QRegularExpressionMatch match;
    bool ok = false;
    uint numbOfCompares = UINT_MAX;
    uint numbOfFormatArgs = UINT_MAX;
    if(interpreterData->arguments.size() < 3)
        return config.ERROR_CALL(Error::ERROR);
    if((numbOfCompares = interpreterData->arguments.at(0).toUInt(&ok), !ok) ||
            numbOfCompares + 1 >= interpreterData->arguments.size()  ||
            (numbOfFormatArgs = interpreterData->arguments.at(numbOfCompares + 1).toUInt(&ok), !ok) ||
            numbOfCompares + numbOfFormatArgs + 2 != interpreterData->arguments.size())
        return config.ERROR_CALL(Error::ERROR);
    if(!interpreterData->createAndAssignString(formatStr, interpreterData->arguments.sliced(2 + numbOfCompares)))
        return config.ERROR_CALL(Error::ERROR);
    for(QStringList::ConstIterator str = interpreterData->arguments.begin() + 1; str < interpreterData->arguments.begin() + 1 + numbOfCompares; str++)
        if((interpreterData->conditionResult = (formatStr.lastIndexOf(QRegularExpression(*str), -1, &match) == 0 && formatStr.size() == match.capturedLength())))
            break;
    return true;
}
*/
template<>template<>template<>
bool FSD_ByLine_TcFileModifierData::processingFunction<FSD_ByLine_TcFileModifierData::Stat::ACTION_SPLIT>(){
    const QString PRE_ERROR_MSG = "Internal Error: Action Split";

    if(interpreterData->arguments.size() < 1)
        return config.ERROR_CALL(PRE_ERROR_MSG + " - Wrong Numb of Action Arguments");

    QString str;
    if(interpreterData->createAndAssignString(str, interpreterData->arguments) != interpreterData->arguments.size())
        return config.ERROR_CALL(PRE_ERROR_MSG);

    interpreterData->lastActionResponse = interpreterData->lineData.split(QRegularExpression(str), Qt::SkipEmptyParts);

    return interpreterData->conditionResult = true;
}

template<>template<>template<>
bool FSD_ByLine_TcFileModifierData::processingFunction<FSD_ByLine_TcFileModifierData::Stat::ACTION_COMMENT_OUT>(){
    //QString command = "\t // " + interpreterData->lineData + "\n";
    //interpreterData->tclToCaplInterpreter.addReadyCommand(command);
    return true;
}

template<>template<>template<>
bool FSD_ByLine_TcFileModifierData::processingFunction<FSD_ByLine_TcFileModifierData::Stat::ACTION_CHANGE_PHASE>(){
    const QString PRE_ERROR_MSG = "Internal Error: Action ChangePhase";
    bool checkRulesAgain = false;
    bool ok = false;
    if(interpreterData->arguments.size() != 2 or
            (interpreterData->arguments.at(0).isEmpty()) or
            (checkRulesAgain = interpreterData->arguments.at(1).toUInt(&ok), !ok))
        return config.ERROR_CALL(PRE_ERROR_MSG);
    interpreterData->curPhase = interpreterData->userConfig_.modifierPhases().constFind(interpreterData->arguments.at(0));
    if(interpreterData->curPhase == interpreterData->userConfig_.modifierPhases().constEnd()){
        return config.ERROR_CALL(PRE_ERROR_MSG + ": Phase not found");
    }
    if(checkRulesAgain)
        config.stats.append(static_cast<int>(Stat::REPLACE_BY_MAPPING) + (int)Parent_FSD::FileSpecificInterpreterStat::SIZE);
    return true;
}

template<>template<>template<>
bool FSD_ByLine_TcFileModifierData::processingFunction<FSD_ByLine_TcFileModifierData::Stat::ACTION_WRITE_ATTRIBUTE>(){
    const QString PRE_ERROR_MSG = "Internal Error: Action WriteAttribute";
    QStringList& parameters = interpreterData->arguments;
    QStringList::size_type result = 0;
    QString formatStr;
    bool ok = false;
    uint numbOfFormatArgs = UINT_MAX;
    if(parameters.size() < 2){
        return config.ERROR_CALL(PRE_ERROR_MSG + "Number of Action Parameters dont match.");
    }
    if(parameters.at(0).isEmpty() or
            (numbOfFormatArgs = parameters.at(1).toUInt(&ok), !ok) or
            numbOfFormatArgs + 2 != parameters.size())
    {
        return config.ERROR_CALL(PRE_ERROR_MSG + "Action Parameters are wrong.");
    }

    QStringList&& params = parameters.mid(2);
    if((result = interpreterData->createAndAssignString(formatStr, params)) != params.size())
        return config.ERROR_CALL(PRE_ERROR_MSG + " - Create Text Error");

    interpreterData->userConfig_.attributes().insert(parameters.at(0), {formatStr});
    return interpreterData->conditionResult = true;
}
/*
template<>template<>template<>
bool FSD_ByLine_TcFileModifierData::processingFunction<FSD_ByLine_TcFileModifierData::Stat::ACTION_SSTR_SAVE>(){
    QString str;
    if(!interpreterData->createAndAssignString(str, interpreterData->arguments))
        return config.ERROR_CALL(Error::ERROR);
    interpreterData->sstr = str;
    return true;
}

template<>template<>template<>
bool FSD_ByLine_TcFileModifierData::processingFunction<FSD_ByLine_TcFileModifierData::Stat::ACTION_SSTR_SLICE>(){
    bool ok = false;
    uint position = 0;
    if(interpreterData->arguments.size() != 1 ||
            (position = interpreterData->arguments.at(0).toUInt(&ok), !ok))
        return config.ERROR_CALL(Error::ERROR);
    interpreterData->sstr = interpreterData->sstr.sliced(position);
    return true;
}
*/
template<>template<>template<>
bool FSD_ByLine_TcFileModifierData::processingFunction<FSD_ByLine_TcFileModifierData::Stat::ACTION_REPLACE_ALL>(){
    const QString PRE_ERROR_MSG = "Internal Error: Action ReplaceAll";
    if(interpreterData->arguments.size() < 2)
        return config.ERROR_CALL(PRE_ERROR_MSG + " - Wrong Numb of Arguments");
    for(QStringList::ConstIterator str = interpreterData->arguments.constBegin() + 1; str < interpreterData->arguments.constEnd(); str++)
        interpreterData->lineData = interpreterData->lineData.replace(*str, interpreterData->arguments.at(0));
    return true;
}


template<>template<>template<>
bool FSD_ByLine_TcFileModifierData::processingFunction<FSD_ByLine_TcFileModifierData::Stat::ACTION_DEBUG>(){
    return interpreterData->conditionResult = true;
}
/*
template<>template<>template<>
bool FSD_ByLine_TcFileModifierData::processingFunction<FSD_ByLine_TcFileModifierData::Stat::ACTION_GET_MSG_BY_SIG_NAME>(){
    QString signalStr;
    QString vtSignalStr;
    bool ok = false;
    uint numbOfSignalArgs = UINT_MAX;
    uint numbOfVtSignalArgs = UINT_MAX;
    if(interpreterData->arguments.size() < 3)
        return config.ERROR_CALL(Error::ERROR);
    if((numbOfSignalArgs = interpreterData->arguments.at(0).toUInt(&ok), !ok) ||
            numbOfSignalArgs + 1 >= interpreterData->arguments.size()  ||
            (numbOfVtSignalArgs = interpreterData->arguments.at(numbOfSignalArgs + 1).toUInt(&ok), !ok) ||
            numbOfVtSignalArgs + numbOfSignalArgs + 2 != interpreterData->arguments.size())
        return config.ERROR_CALL(Error::ERROR);
    if(!interpreterData->createAndAssignString(signalStr, interpreterData->arguments.sliced(1, numbOfSignalArgs))
            || !interpreterData->createAndAssignString(vtSignalStr, interpreterData->arguments.sliced(2 + numbOfSignalArgs)))
        return config.ERROR_CALL(Error::ERROR);
    if(CAN::_signals.contains(signalStr)){
        const QStringList& vtSignals = CAN::_signals.value(signalStr);
        for(QStringList::ConstIterator vtSignal = vtSignals.constBegin(); vtSignal < vtSignals.constEnd(); vtSignal++){
            if((interpreterData->conditionResult = (*vtSignal == vtSignalStr))){
                break;
            }
        }
    }
    return true;
}

template<>template<>template<>
bool FSD_ByLine_TcFileModifierData::processingFunction<FSD_ByLine_TcFileModifierData::Stat::ACTION_ADD_VARIABLE>(){
    if(interpreterData->lastActionResponse.size() >= 3){
        QString varName = interpreterData->lastActionResponse.at(1);
        QString instruction;
        interpreterData->variables.append(varName);
        /* _C_ if(!interpreterData->createAndAssignString(instruction, {"\t" + varName + " = ", "", "@ ", "", ">1", "; // _VAR_ Check\n"}))
            return config.ERROR_CALL(Error::ERROR);*//*
        if(!interpreterData->createAndAssignString(instruction, {"\t\\ _PH_IF_  ", "", "T" + Format::cast_target_str(Format::Target::RAW), "", "=", "\n"}))
            return config.ERROR_CALL(Error::ERROR);
        interpreterData->tcContent += instruction;*//*
    }
    return true;
}
*/

template<>template<>template<>
bool FSD_ByLine_TcFileModifierData::processingFunction<FSD_ByLine_TcFileModifierData::Stat::ACTION_INTERPRET>(){
    const QString PRE_ERROR_MSG = "Internal Error: Action Interpret";
    QString str;
    if( interpreterData->createAndAssignString(str, interpreterData->arguments) != interpreterData->arguments.size())
        return config.ERROR_CALL(PRE_ERROR_MSG);
    //str += "\n";
    if(not str.isEmpty())
    {
        if(interpreterData->tclToCaplInterpreter_.toCAPL(str) == Core::Error::Error){
            interpreterData->tclToCaplInterpreter_.addIgnoreMessage("CRITICAL ERROR: File is not interpreted by TCL Interpreter after previous TCL interpreter error.");
            return config.ERROR_CALL(PRE_ERROR_MSG + " - TCL Interpreter Critical Error");
        }
    }

    return true;
}

// Internal Functions


int FSD_ByLine_TcFileModifierData::Data::createAndAssignString(QString &dest, QStringList args){
    using Target = Format::Target;
    Target target = Target::SPLITTED_RAW;
    QString* targetStr = nullptr;
    QString seperator;
    bool separatorUsed = false;
    for(QStringList::Iterator arg = args.begin(); arg < args.end(); arg++){
        // For Empty String next arg is "special", "control string":
        // - "=<number>", ex. "=2" - take string from lastActionResponse at index 2
        // - "><number>", ex. ">2" - take all string from lastActionResponse in index range: 2 ...
        // - "<sign><number>@", ex. ">2@ " - after @ place separator between arguments
        int index = INT_MAX;
        bool ok = false;
        if(arg->isEmpty()){ // "Special Argument"
            arg++;
            if(arg < args.end()){
                // Format Rules
                    using Rule = Format::Rule;
                    Rule formatRule;
                    switch((formatRule = static_cast<Rule>(arg->at(0).toLatin1()), *arg = arg->mid(1), formatRule)){
                    case Rule::SEPARATOR:
                    {
                        seperator = *arg;
                        separatorUsed = false;
                    }
                        break;
                    case Rule::INDEX_OR_FULL_LINE:
                    {
                        if(arg->isEmpty()){
                            // FULL LINE ----------------
                            switch(target){
                            case Target::RAW:
                            //case Target::SSTR:
                                if(!targetStr)
                                    return (arg - args.begin());
                                dest += *targetStr;
                                break;
                            default:
                                return (arg - args.begin());
                            }
                            // FULL LINE ----------------
                        }else{                            
                            // ARGUMENT AT INDEX -------------------------------
                            if((index = arg->toInt(&ok), !ok))
                                return (arg - args.begin());
                            switch(target){
                            case Target::RAW:
                            //case Target::SSTR:
                                if(!targetStr)
                                    return (arg - args.begin());
                                if(index < 0){// For index < 0, recalculate index by: size of lastResponse + index -> Then check if index in range
                                    index = targetStr->size() + index; // Index is negative
                                }
                                if(index < 0 || index >= targetStr->size())
                                    return (arg - args.begin());
                                dest += targetStr->at(index);
                                break;
                            case Target::SPLITTED_RAW:
                                if(index < 0){// For index < 0, recalculate index by: size of lastResponse + index -> Then check if index in range
                                    index = lastActionResponse.size() + index; // Index is negative
                                }
                                if(index < 0 || index >= lastActionResponse.size())
                                    return (arg - args.begin());
                                dest += lastActionResponse.at(index);
                                break;
                            default:
                                return (arg - args.begin());
                            }
                            // ARGUMENT AT INDEX -------------------------------
                        }
                    }
                        break;
                    case Rule::ARGS_AFTER_INDEX:
                    {
                        // ALL ARGUMENTS AFTER INDEX -------------------------------
                        if((index = arg->toInt(&ok), !ok))
                            return (arg - args.begin());
                        switch(target){
                        case Target::RAW:
                        //case Target::SSTR:
                        {
                            if(index < 0){// For index < 0, recalculate index by: size of lastResponse + index -> Then check if index in range
                                index = targetStr->size() + index; // Index is negative
                            }
                            if(index < 0 || index >= targetStr->size())
                                return (arg - args.begin());
                            dest += targetStr->mid(index);
                        }
                            break;
                        case Target::SPLITTED_RAW:
                        {
                            if(index < 0){// For index < 0, recalculate index by: size of lastResponse + index -> Then check if index in range
                                index = lastActionResponse.size() + index; // Index is negative
                            }
                            if(index < 0 || index >= lastActionResponse.size())
                                return (arg - args.begin());
                            for(QStringList::Iterator responseArg = lastActionResponse.begin() + index ; responseArg < lastActionResponse.end(); responseArg++){
                                dest += *responseArg + seperator;
                                separatorUsed = true;
                            }
                        }
                            break;
                        default:
                            return (arg - args.begin());
                        }
                        // ALL ARGUMENTS AFTER INDEX -------------------------------
                    }
                        break;
                    case Rule::ATTRIBUTE:
                    {
                        if(arg->isEmpty())
                            return (arg - args.begin());
                        ControllerConfigInfo::Attributes::ConstIterator attribute = userConfig_.attributes().constFind(*arg);
                        if(attribute == userConfig_.attributes().constEnd())
                            return (arg - args.begin());
                        dest += attribute.value().value;
                    }
                        break;
                    /*case Rule::ARG_IN_RANGE_P1:
                    {
                        switch((formatRule = static_cast<Rule>(arg->at(0).toLatin1()), *arg = arg->sliced(1), formatRule)){
                        case Rule::ARG_IN_RANGE_P2:
                        {
                            // ALL ARGUMENTS IN RANGE -------------------------------
                            int firstIndex = INT_MAX;
                            int secondIndex = INT_MAX;
                            QStringList indexes;
                            if((indexes = arg->split(";"), indexes.size() != 2) ||
                                    (firstIndex = indexes.at(0).toInt(&ok), !ok) ||
                                    (secondIndex = indexes.at(1).toInt(&ok), !ok))
                                return false;
                            switch(target){
                            case Target::RAW:
                            case Target::SSTR:
                            {
                                if(!targetStr)
                                    return false;
                                if(firstIndex < 0){// For index < 0, recalculate index by: size of lastResponse + index -> Then check if index in range
                                    firstIndex = targetStr->size() + firstIndex; // Index is negative
                                }
                                if(secondIndex < 0){// For index < 0, recalculate index by: size of lastResponse + index -> Then check if index in range
                                    secondIndex = targetStr->size() + secondIndex; // Index is negative
                                }
                                if(firstIndex < 0 || firstIndex >= targetStr->size() ||
                                        secondIndex < 0 || secondIndex >= targetStr->size() ||
                                        firstIndex >= secondIndex)
                                     return false;
                                for(QString::Iterator responseArg = targetStr->begin() + firstIndex; responseArg < targetStr->begin() + secondIndex; responseArg++){
                                    dest += *responseArg + seperator;
                                    separatorUsed = true;
                                }
                            }
                                break;
                            case Target::SPLITTED_RAW:
                            {
                                if(firstIndex < 0){// For index < 0, recalculate index by: size of lastResponse + index -> Then check if index in range
                                    firstIndex = lastActionResponse.size() + firstIndex; // Index is negative
                                }
                                if(secondIndex < 0){// For index < 0, recalculate index by: size of lastResponse + index -> Then check if index in range
                                    secondIndex = lastActionResponse.size() + secondIndex; // Index is negative
                                }
                                if(firstIndex < 0 || firstIndex >= lastActionResponse.size() ||
                                        secondIndex < 0 || secondIndex >= lastActionResponse.size() ||
                                        firstIndex >= secondIndex)
                                     return false;
                                for(QStringList::Iterator responseArg = lastActionResponse.begin() + firstIndex; responseArg < lastActionResponse.begin() + secondIndex; responseArg++){
                                    dest += *responseArg + seperator;
                                    separatorUsed = true;
                                }
                            }
                                break;
                            default:
                                return false;
                            }
                            // ALL ARGUMENTS IN RANGE -------------------------------
                        }
                            break;
                        default:
                            return false;
                        }
                    }
                        break;
                    case Rule::CUT_INDEX:
                    {
                        switch(target){
                        case Target::RAW:
                        case Target::SSTR:
                        {
                            return false;   // Not Implemented yet
                        }
                            break;
                        case Target::SPLITTED_RAW:
                        {
                            int cutWidth = 0;
                            QStringList indexes;
                            if((indexes = arg->split(";"), indexes.size() != 2) ||
                                    (index = indexes.at(0).toInt(&ok), !ok) || index < 0 || index >= lastActionResponse.size()||
                                    (cutWidth = indexes.at(1).toInt(&ok), !ok) || cutWidth < 1 || cutWidth > lastActionResponse.at(index).size() )
                                return false;
                            int lastI = lastActionResponse.at(index).size() - cutWidth;
                            for(int i = 0; i <= lastI; i+= cutWidth){
                                dest += lastActionResponse.at(index).sliced(i, cutWidth) + seperator;
                                separatorUsed = true;
                            }
                        }
                            break;
                        default:
                            return false;
                        }
                    }
                        break;
                    case Rule::CUT_AFTER_INDEX:
                    {
                        switch(target){
                        case Target::RAW:
                        case Target::SSTR:
                        {
                            return false;   // Not Implemented yet
                        }
                            break;
                        case Target::SPLITTED_RAW:
                        {
                            int cutWidth = 0;
                            QStringList indexes;
                            if((indexes = arg->split(";"), indexes.size() != 2) ||
                                    (index = indexes.at(0).toInt(&ok), !ok) || index < 0 || index >= lastActionResponse.size()||
                                    (cutWidth = indexes.at(1).toInt(&ok), !ok) || cutWidth < 1 || cutWidth > lastActionResponse.at(index).size() )
                                return false;
                            for(QStringList::Iterator responseArg = lastActionResponse.begin() + index + 1; responseArg < lastActionResponse.end(); responseArg++){
                                int lastI = responseArg->size() - cutWidth;
                                for(int i = 0; i <= lastI; i+= cutWidth){
                                    dest += responseArg->sliced(i, cutWidth) + seperator;
                                    separatorUsed = true;
                                }
                            }
                        }
                            break;
                        default:
                            return false;
                        }
                    }
                        break;
                        */
                    case Rule::TARGET:
                    {
                        if((target = static_cast<Target>(arg->toUInt(&ok)), !ok))
                            return (arg - args.begin());
                        switch (target) {
                        case Target::RAW:
                            targetStr = &lineData;
                        case Target::SPLITTED_RAW:
                            break;
                        /*case Target::SSTR:
                            targetStr = &sstr;
                            break;*/
                        default:
                            return (arg - args.begin());
                        }
                    }
                        break;

                    /*case Rule::SPLIT:
                    {
                        switch (target) {
                        case Target::RAW:
                        case Target::SSTR:
                            if(!targetStr)
                                return false;
                            lastActionResponse = targetStr->split(QRegularExpression(*arg), Qt::SkipEmptyParts);
                            break;
                        case Target::SPLITTED_RAW:
                        default:
                            return false;
                        }
                    }
                        break;
                    case Rule::SLICED:
                    {
                        switch (target) {
                        case Target::RAW:
                        case Target::SSTR:
                        {
                            if(!targetStr)
                                return false;
                            if(index < 0){// For index < 0, recalculate index by: size of lastResponse + index -> Then check if index in range
                                index = targetStr->size() + index; // Index is negative
                            }
                            if(index < 0 || index >= targetStr->size())
                                return false;
                            *targetStr = targetStr->sliced(index);
                        }
                            break;
                        case Target::SPLITTED_RAW:
                        {
                            int firstIndex = INT_MAX;
                            int secondIndex = INT_MAX;
                            QStringList indexes;
                            if((indexes = arg->split(";"), indexes.size() != 2) ||
                                    (firstIndex = indexes.at(0).toInt(&ok), !ok) ||
                                    (secondIndex = indexes.at(1).toInt(&ok), !ok))
                                return false;
                            if(firstIndex < 0){// For index < 0, recalculate index by: size of lastResponse + index -> Then check if index in range
                                firstIndex = lastActionResponse.size() + firstIndex; // Index is negative
                            }
                            if(firstIndex < 0 || firstIndex >= lastActionResponse.size())
                                return false;
                            if(secondIndex < 0){// For index < 0, recalculate index by: size of lastResponse + index -> Then check if index in range
                                secondIndex = lastActionResponse.at(firstIndex).size() + secondIndex; // Index is negative
                            }
                            if(secondIndex < 0 || secondIndex >= lastActionResponse.at(firstIndex).size())
                                return false;
                            lastActionResponse[firstIndex] = lastActionResponse.at(firstIndex).sliced(secondIndex);
                        }
                            break;
                        default:
                            return false;
                        }
                    }
                        break;*/
                    default:
                        return (arg - args.begin());
                    }
                if(separatorUsed){
                    dest.chop(seperator.size());
                    seperator = QString();
                    separatorUsed = false;
                }
            }else{
                arg--;
            }

        }else{
            dest += *arg;
        }
    }

    return args.size();
}

/*
template<>template<>template<>
bool FSD_ByLine_TcFileModifierData::processingFunction<FSD_ByLine_TcFileModifierData::Stat::ACTION_CONTAINS>(){
    if(interpreterData->arguments.size() < 1)
        return config.ERROR_CALL(Error::ERROR);
    for(QStringList::ConstIterator str = interpreterData->arguments.constBegin(); str < interpreterData->arguments.constEnd(); str++)
        if((interpreterData->conditionResult = interpreterData->lineData.contains(*str)))
            break;
    return true;
}

template<>template<>template<>
bool FSD_ByLine_TcFileModifierData::processingFunction<FSD_ByLine_TcFileModifierData::Stat::ACTION_REMOVE_SPLIT_RESULT_IF_ENDS_WITH>(){
    if(interpreterData->arguments.size() < 1)
        return config.ERROR_CALL(Error::ERROR);
    for(QStringList::Iterator str = interpreterData->lastActionResponse.begin(); str < interpreterData->lastActionResponse.end(); str++)
    {
        for(QStringList::ConstIterator arg = interpreterData->arguments.constBegin(); arg < interpreterData->arguments.constEnd(); arg++)
            if(str->endsWith(*arg))
            {
                interpreterData->lastActionResponse.removeAt(str - interpreterData->lastActionResponse.begin());
                str--;
                break;
            }
    }
    return true;
}
*/

void FSD_ByLine_TcFileModifierData::Data::writeTCInfo(FSD_ByLine_TcFileModifierData::DataModel& dataModel){
     if(not tclToCaplInterpreter_.isPredefinitionMode()){
        QString tcInfo;
        tcInfo += "testcase " + tcData.name +"(){\n"
      "// " + tcData.name + "\n"
      "//============================== TEST CASE HEADER ==============================//\n"
      "/*\n"
      "Set criteria: (short circuit from any speaker output to Vbatt) OR (short circuit from any speaker output to gnd) OR (overload detected) while (amplifier is on)\n"
      "Reset criteria: (No short circuit from any speaker output nor overload detected) while (amplifier is on)\n"
      "*/\n"
      "// Internal variables:\n";

        // _BACK_ variables.removeDuplicates();
        // _BACK_ for(QStringList::Iterator var = variables.begin(); var < variables.end(); var++)
        // _BACK_     tcInfo += "long " + *var + "; // _VAR_ Check\n";

        tcInfo += tclToCaplInterpreter_.printPredefinitions() + "\n";

        tcInfo += "\n"
      "// Test Case description and attributes:\n"
    "  AddTcInformation(\n"
    "  /*ident     */  \"" + tcData.name +"\",\n"
    "  /*title     */  \"" + ((tcData.title.isEmpty())? tcData.description : tcData.title)  + "\",\n"
    "  /*descr     */  \"" + tcData.description + "\"\n"
    "  );\n"
    "  AddTcInAttributes(\n"
    "  /*domain    */  \"" + tcData.domain + "\",\n"
    "  /*reqs      */  \"" + tcData.requirements + "\",\n"
    "  /*doc       */  \"" + tcData.documents + "\",\n"
    "  /*reviewed  no/yes */  \"no\",\n"
    "  /*automated no/yes */  \"" + ((tcData.type == "AUTO")? "yes" : "no") + "\",\n"
    "  /*priority  1/2/3  */  \"1\",\n"
    "  /*intg      no/yes */  \"" + ((tcData.integration == "NO")? "no" : "yes") + "\",\n"
    "  /*variant   */  \"" + fileDir.dirName() + "\",\n"
    "  /*version   */  \"" + tcData.version + "\",\n"
    "  /*author    */  \"" + tcData.author + "\"\n"
    "  );\n\n ";
        dataModel.write(tcInfo + tclToCaplInterpreter_.readCommand().replace(QRegularExpression("\n(?=\\S*)(?!\\S*}\\Z)"), "\n\t") + "\n}\n\n");
   }

}


template<>
template<>
QVector<ProcessingFunctions_FRI<FSD_ByLine_TcFileModifierData::Config>> FSD_ByLine_TcFileModifierData::processingFunctions = {
    &FSD_ByLine_TcFileModifierData::processingFunction<FSD_ByLine_TcFileModifierData::Stat::REPLACE_BY_MAPPING>,
    &FSD_ByLine_TcFileModifierData::processingFunction<FSD_ByLine_TcFileModifierData::Stat::ACTION_SPLIT>,
    &FSD_ByLine_TcFileModifierData::processingFunction<FSD_ByLine_TcFileModifierData::Stat::ACTION_WRITE_ATTRIBUTE>,
    &FSD_ByLine_TcFileModifierData::processingFunction<FSD_ByLine_TcFileModifierData::Stat::ACTION_STARTS_WITH>,
    &FSD_ByLine_TcFileModifierData::processingFunction<FSD_ByLine_TcFileModifierData::Stat::ACTION_COMPARE>,
    &FSD_ByLine_TcFileModifierData::processingFunction<FSD_ByLine_TcFileModifierData::Stat::ACTION_WRITE>,
    &FSD_ByLine_TcFileModifierData::processingFunction<FSD_ByLine_TcFileModifierData::Stat::ACTION_CHANGE_PHASE>,
    &FSD_ByLine_TcFileModifierData::processingFunction<FSD_ByLine_TcFileModifierData::Stat::ACTION_INTERPRET>,
    //&FSD_ByLine_TcFileModifierData::processingFunction<FSD_ByLine_TcFileModifierData::Stat::ACTION_ENDS_WITH>,
    &FSD_ByLine_TcFileModifierData::processingFunction<FSD_ByLine_TcFileModifierData::Stat::ACTION_WRITE_TO_TC_INFO>,
    &FSD_ByLine_TcFileModifierData::processingFunction<FSD_ByLine_TcFileModifierData::Stat::ACTION_COMMENT_OUT>,
    /*&FSD_ByLine_TcFileModifierData::processingFunction<FSD_ByLine_TcFileModifierData::Stat::ACTION_SSTR_SAVE>,
    &FSD_ByLine_TcFileModifierData::processingFunction<FSD_ByLine_TcFileModifierData::Stat::ACTION_SSTR_SLICE>,*/
    &FSD_ByLine_TcFileModifierData::processingFunction<FSD_ByLine_TcFileModifierData::Stat::ACTION_REPLACE_ALL>,
    &FSD_ByLine_TcFileModifierData::processingFunction<FSD_ByLine_TcFileModifierData::Stat::ACTION_DEBUG>,
    //&FSD_ByLine_TcFileModifierData::processingFunction<FSD_ByLine_TcFileModifierData::Stat::ACTION_FORMAT>,
    /*&FSD_ByLine_TcFileModifierData::processingFunction<FSD_ByLine_TcFileModifierData::Stat::ACTION_COMPARE_REGEX>,
    &FSD_ByLine_TcFileModifierData::processingFunction<FSD_ByLine_TcFileModifierData::Stat::ACTION_FORMAT>,
    &FSD_ByLine_TcFileModifierData::processingFunction<FSD_ByLine_TcFileModifierData::Stat::ACTION_CONTAINS>,
    &FSD_ByLine_TcFileModifierData::processingFunction<FSD_ByLine_TcFileModifierData::Stat::ACTION_REMOVE_SPLIT_RESULT_IF_ENDS_WITH>,
    &FSD_ByLine_TcFileModifierData::processingFunction<FSD_ByLine_TcFileModifierData::Stat::ACTION_GET_MSG_BY_SIG_NAME>,
    &FSD_ByLine_TcFileModifierData::processingFunction<FSD_ByLine_TcFileModifierData::Stat::ACTION_ADD_VARIABLE>,*/
};

template<>
template<>
bool FSD_ByLine_TcFileModifierData::Config::FSFunction<FSD_ByLine::FileSpecificInterpreterStat::PROCESS_LINE>(){
    const QString PRE_ERROR_MSG = "Internal Error: ProcessLine Stat";
    if(not FRI_COM_Manager::next(COMChannel))
        return ERROR_CALL(PRE_ERROR_MSG + " - End of COMChannelData");
    NS_FRI_COM_Manager::ChannelData& dataCmd = FRI_COM_Manager::receive(COMChannel);
    if(dataCmd.command != NS_FRI_COM_Manager::Commands::DATA || !data)
        return ERROR_CALL(PRE_ERROR_MSG + " - Wrong COM Command");
    interpreterData->curLine++;
    interpreterData->lineData = *static_cast<QString*>(dataCmd.channelData);//->split(" ", Qt::SplitBehaviorFlags::SkipEmptyParts);
    stats.append(static_cast<int>(Stat::REPLACE_BY_MAPPING) + (int)Parent_FSD::FileSpecificInterpreterStat::SIZE);

    return true;
}


template<>
QVarLengthArray<ProcessingFunctions_FRI<FSD_ByLine_TcFileModifierData::Config>, (int)FSD_ByLine_TcFileModifierData::Parent_FSD::FileSpecificInterpreterStat::SIZE? (int)FSD_ByLine_TcFileModifierData::Parent_FSD::FileSpecificInterpreterStat::SIZE : 1> FSD_ByLine_TcFileModifierData::Config::processingFunctions_FS = {
    &FSD_ByLine_TcFileModifierData::Config::FSFunction<FSD_ByLine::FileSpecificInterpreterStat::PROCESS_LINE>,
};
