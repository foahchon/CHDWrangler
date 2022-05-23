#pragma once
#ifndef CHDMANPROGRESSTHREAD_H
#define CHDMANPROGRESSTHREAD_H

#include <wx/process.h>
#include <wx/thread.h>
#include <wx/msgqueue.h>
#include <wx/txtstrm.h>

#include <CHDManProgressEvent.h>

wxDECLARE_EVENT(wxEVT_THREAD_STDIN, wxThreadEvent);
wxDECLARE_EVENT(wxEVT_THREAD_STDERR, wxThreadEvent);

class CHDManProgressThread : public wxThread
{
    public:
        enum ThreadMessage
        {
            ProcessComplete,
            ExitThread,
            MessageLast
        };
        CHDManProgressThread(wxProcess *process, wxEvtHandler *threadEventHandler, wxMessageQueue<ThreadMessage> *messageQueue);
        ~CHDManProgressThread();

    private:
        wxThread::ExitCode Entry() wxOVERRIDE;

        wxMessageQueue<ThreadMessage> *m_messageQueue;
        char *m_readBuffer;
        wxProcess *m_process;
        wxEvtHandler *m_threadEventHandler;
};

#endif /* CHDMANPROGRESSTHREAD_H */