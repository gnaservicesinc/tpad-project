#ifndef TPAD_QT_WRITING_TOOLS_MAC_H
#define TPAD_QT_WRITING_TOOLS_MAC_H

#ifndef TPAD_USE_QT
#  error "The Qt frontend must be compiled with TPAD_USE_QT"
#endif

class QWidget;

bool macWritingToolsAvailable();
void showMacWritingTools(QWidget *focusWidget);

#endif
